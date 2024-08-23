// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniVehicleBus.h"

#include "OmniAICtrlBus.h"
#include "Omnibus.h"
#include "OmnibusPlayData.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniLineBusRoute.h"
#include "OmniPassenger.h"
#include "OmniStationBusStop.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/TextRenderActor.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"

AOmniVehicleBus::AOmniVehicleBus()
{
	PrimaryActorTick.bCanEverTick = true;

	SetCanAffectNavigationGeneration(false);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	PawnMovement = CreateDefaultSubobject<USpectatorPawnMovement>(TEXT("MovementComponent"));
	PawnMovement->UpdatedComponent  = RootComponent;
	PawnMovement->bConstrainToPlane = true;
	PawnMovement->MaxSpeed          = 900.0f;
	PawnMovement->Acceleration      = 900.0f;
	PawnMovement->Deceleration      = 120000.0f;
	PawnMovement->TurningBoost      = 50.0f;
	PawnMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);

	BusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BusMesh"));
	BusMesh->SetupAttachment(RootComponent);
	BusMesh->SetCanEverAffectNavigation(false);
	BusMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	ForwardSensingBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Sensor"));
	ForwardSensingBoxComponent->SetupAttachment(RootComponent);
	ForwardSensingBoxComponent->SetRelativeLocation(FVector(150, 0.0, 0.0));
	ForwardSensingBoxComponent->SetCollisionProfileName(EOmniCollisionProfile::BusDetectorOverlap);
	
	AIControllerClass = AOmniAICtrlBus::StaticClass();
	AutoPossessAI     = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 기준값
	OwnerBusRoute    = nullptr;
	SteeringDistance = 100.0f;
	CarSpec          = FCarSpec();

	// 변경 가능
	ThisStopIndex        = INDEX_NONE;
	CurrentRouteDistance = 0.0f;
	NowRouteSpeed        = PawnMovement->MaxSpeed;

	bEnterDecelArea = false;
	bBoarding       = false;
}

void AOmniVehicleBus::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if(ForwardSensingBoxComponent)
	{
		const double BoxDistance = SteeringDistance + ForwardSensingBoxComponent->GetScaledBoxExtent().X;
		ForwardSensingBoxComponent->SetRelativeLocation(FVector(BoxDistance, 0.0, 0.0));
	}
}

void AOmniVehicleBus::BeginPlay()
{
	Super::BeginPlay();

	// 버스 노선없이 스폰된 경우, 로그 남기고 파괴
	if (OB_IS_WEAK_PTR_VALID(OwnerBusRoute) == false || ThisStopIndex == INDEX_NONE)
	{
		OB_LOG("This Bus was spawned alone without an 'OwnerBusRoute'.")
		Destroy();
		return;
	}

#if WITH_EDITORONLY_DATA
	BusLineTargetTextRender = SimpleTextRender(GetActorLocation(), 0.0, TEXT("0"), 200.0f, 150.0);
#endif // WITH_EDITORONLY_DATA
}

void AOmniVehicleBus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DriveOnBusLine(DeltaTime);
}

void AOmniVehicleBus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOmniVehicleBus::SetupSpawnInit(AOmniLineBusRoute* InOwnerRoute, const ECarType InSpawnCarType, const float InSpawnRouteDist)
{
	OwnerBusRoute        = InOwnerRoute;
	ThisStopIndex        = InOwnerRoute->FindThisStopIdxByDist(InSpawnRouteDist);
	CurrentRouteDistance = OmniMath::CircularNumF(InOwnerRoute->GetRouteSpline()->GetSplineLength(), InSpawnRouteDist + SteeringDistance); // 약간 더 전방을 바라봄.

	CarSpec = GetOmnibusPlayData()->GetCarSpecList(InSpawnCarType);
	OB_IF(CarSpec.CarType != InSpawnCarType)
	{
		Destroy();
	}

	SetDriveMaxSpeed(CarSpec.MaxSpeed);

	const float BaseSpeed = GetOmnibusPlayData()->GetBaseSpeed();
	// 최대 속도에 따른 비율 유지
	PawnMovement->Acceleration = CarSpec.Acceleration /** 10.0f*/;
	PawnMovement->Deceleration = (CarSpec.MaxSpeed / BaseSpeed) * 16000.f;
	PawnMovement->TurningBoost = (CarSpec.MaxSpeed / BaseSpeed) * 50.f;
}

void AOmniVehicleBus::StartBoarding()
{
	bBoarding = true;

	AOmniStationBusStop* ThisBusStop = GetThisStopDist().BusStop.Get();
	if (ThisBusStop == nullptr)
		return;

	// 하차 먼저.
	TArray<AOmniPassenger*>	ExitPassengerList;
	PassengerList.RemoveAll([&ExitPassengerList, ThisBusStop](const TWeakObjectPtr<AOmniPassenger>& PassengerWeak)
	{
		// Invalid 제거.
		AOmniPassenger* Passenger = PassengerWeak.Get();
		if (Passenger == nullptr)
			return true;

		if (Passenger->IsExitToThisBusStop(ThisBusStop))
		{
			ExitPassengerList.Emplace(Passenger);
			return true;
		}
		return false;
	});

	// 단위 하차 시간
	const float UnitExitDelay = 1.0f / CarSpec.ExitSpeed;
	for (int idx = 0; idx < ExitPassengerList.Num(); ++idx)
	{
		// 각각 일정 시간 간격으로 하차 절차 밟음.
		const float ExitDelay = static_cast<float>(idx + 1) * UnitExitDelay;
		
		AOmniPassenger* ExitPassenger = ExitPassengerList[idx];
		ExitPassenger->DoExitToStop(ThisBusStop, ExitDelay);
	}

	// 하차 완료 예정 시간 이후, 승차 절차 시작. 없다면 즉시 시작.
	if (ExitPassengerList.IsEmpty())
	{
		ThisBusStop->StartEntryToBus(this);
	}
	else
	{
		const float EntryTime = static_cast<float>(ExitPassengerList.Num()) * UnitExitDelay;
		FTimerHandle TempTimer;
		GetWorldTimerManager().SetTimer(TempTimer, [ThisBusStop, this]()
		{
			ThisBusStop->StartEntryToBus(this);
		}, EntryTime, false);
	}
}

void AOmniVehicleBus::EndBoarding()
{
	// 승차 종료 후, 타이머를 통한 중복 실행방지.
	if (bBoarding == false)
		return;

	bEnterDecelArea = false;
	bBoarding       = false;

	const AOmniLineBusRoute* BusRoute = OwnerBusRoute.Get();
	if (BusRoute == nullptr)
		return;

	// 다음 정류장 정보로 갱신
	CurrentRouteDistance = GetThisStopDist().Distance;
	ThisStopIndex        = BusRoute->GetNextBusStopIdx(ThisStopIndex);
}

FTransform AOmniVehicleBus::EntryToBus(AOmniPassenger* InPassenger)
{
	PassengerList.Emplace(InPassenger);
	
	return GetActorTransform();
}

bool AOmniVehicleBus::HasSameTransStep(const FTransferStep& InStep) const
{
	const AOmniLineBusRoute* InStepBusLine = InStep.BusLine.Get();
	if (InStepBusLine == nullptr)
		return false;

	AOmniLineBusRoute* MyLine = GetOwnerBusRouteWeak().Get();
	if (MyLine == nullptr)
		return false;

	if (InStepBusLine == MyLine)
	{
		// 정확히 일치
		if (InStep.StartBusStopDistanceIdx == GetThisStopIndex())
			return true;

		// 이번에 탈 버스가 아님.
		return false;
	}

	AOmniStationBusStop* InStepEndStop = InStep.GetEndBusStop();
	if (InStepEndStop == nullptr)
		return false;

	// 도착 정류장이 있는지 확인
	TArray<int32> FindEndStopIdxList = MyLine->FindBusStopIdxList(InStepEndStop);
	if (FindEndStopIdxList.IsEmpty())
		return false;

	// 오차 범위를 더한 값보다 짧은 경로라면 탑승.
	// todo: 길이만 확인 중. 추후 노선 종류에 따라 이동 시간이 다를 수 있음을 고려해야함.
	const float Tolerance = InStep.ExpectedDist * 0.05f;
	for (const int32 EndStopIdx : FindEndStopIdxList)
	{
		const float ShortLength = MyLine->GetShortPathLength(GetThisStopIndex(), EndStopIdx);
		if (ShortLength + Tolerance <= InStep.ExpectedDist)
			return true;
	}

	return false;
}

float AOmniVehicleBus::GetNearestRouteDistance(const FVector& InLocation) const
{
	const AOmniLineBusRoute* BusRoute = OwnerBusRoute.Get();
	if (BusRoute == nullptr)
		return 0.0f;

	const USplineComponent* RouteSpline = BusRoute->GetRouteSpline();
	const float ClosestInputKey         = RouteSpline->FindInputKeyClosestToWorldLocation(InLocation);
	return RouteSpline->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);
}

void AOmniVehicleBus::DriveOnBusLine(const float DeltaTime)
{
	// 승차 중
	if (bBoarding)
		return;

	// 정차 중
	if (bEnterDecelArea)
	{
		float ThisStopDist = GetThisStopDist().Distance;

		// 다음 정류장이 시작 정류장인 경우, 순환하는 것을 고려함. 
		if (ThisStopIndex == 0 && CurrentRouteDistance > ThisStopDist)
			ThisStopDist += GetOwnerRouteLength();

		if (FMath::IsNearlyEqual(ThisStopDist, CurrentRouteDistance, 10.f))
		{
			StartBoarding();
			SetDriveMaxSpeed(0.0f);
			NowRouteSpeed = 0.0f;
			return;
		}
		constexpr float DecelerationFactor = 1.5f;

		CurrentRouteDistance = FMath::FInterpTo(CurrentRouteDistance, ThisStopDist, DeltaTime, DecelerationFactor);
		CurrentRouteDistance = OmniMath::CircularNumF(GetOwnerRouteLength(), CurrentRouteDistance);
	}
	else
	{
		// 목표 Dist 계산
		UpdateCurrentRouteDist(DeltaTime);
	}

	const FVector CurrentRouteLocation = GetCurrentRouteLocation();

	SetBusSpeed(CurrentRouteLocation);
	AddActorLocalRotation(GetLookAtDeltaRotation(CurrentRouteLocation));
	AddMovementInput(GetActorForwardVector());


#if WITH_EDITORONLY_DATA
	DebugBusSpeed = PawnMovement->Velocity.Length();
	BusLineTargetTextRender->SetActorLocation(CurrentRouteLocation + FVector(0, 0, 150));
#endif //WITH_EDITORONLY_DATA
}

void AOmniVehicleBus::UpdateCurrentRouteDist(const float DeltaTime)
{
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	const USplineComponent* SplineComp = GetOwnerRouteSpline();
	if (SplineComp == nullptr)
		return;

	// 정류장에 근접하면, 더이상 Distance 계산 안하고 감속.
	const float SplineLen    = GetOwnerRouteLength();
	const float ThisStopDist = GetThisStopDist().Distance;
	const float DecelDist    = ThisStopDist - CarSpec.StopSpacing; // 감속 구간
	
	if (ThisStopIndex == 0 && CurrentRouteDistance > ThisStopDist)
	{
		// 순환 고려.
		// 1. thisStop이 시작 정류장이고,
		// 2. 현재 Dist가 thisStop보다 값이 큼 ( 현재 위치가 0.0 이전 )
		// 3. 0.0 이전에서 감속 구간이 시작됨
		if (DecelDist < 0.0f && CurrentRouteDistance >= OmniMath::CircularNumF(SplineLen, DecelDist))
		{
			// SimpleTextRender(GetActorLocation(), 10.f, "0", 200);
			bEnterDecelArea = true;
			return;
		}
	}
	else if (CurrentRouteDistance >= DecelDist)
	{
		// SimpleTextRender(GetActorLocation(), 10.f, "0", 200);
		bEnterDecelArea = true;
		return;
	}

	// 코너에서 감속. 90도(PI/2.0 rad)에서 최대 감속
	const float PrevDist           = CurrentRouteDistance;
	const float HalfSteeringDist   = SteeringDistance / 2.0f;
	const FVector PrevTangent      = SplineComp->GetTangentAtDistanceAlongSpline(OmniMath::CircularNumF(SplineLen, PrevDist - HalfSteeringDist), CoordSpace);
	const FVector NextTangent      = SplineComp->GetTangentAtDistanceAlongSpline(OmniMath::CircularNumF(SplineLen, PrevDist + HalfSteeringDist), CoordSpace);
	const float DeltaRadian        = abs(acosf(FVector::DotProduct(PrevTangent.GetSafeNormal(), NextTangent.GetSafeNormal())));
	constexpr float FactorPartRate = 0.7f; // 감속 영향력 
	const float ChangeRateFactor   = FMath::Clamp((1.0f - (DeltaRadian / UE_HALF_PI)) * FactorPartRate, 0.0f, FactorPartRate) + (1.0f - FactorPartRate);


	NowRouteSpeed         = FMath::Clamp(NowRouteSpeed + (CarSpec.Acceleration * DeltaTime * ChangeRateFactor), 0.0f, CarSpec.MaxSpeed);
	const float DeltaDist = NowRouteSpeed * DeltaTime;
	const float NextDist  = OmniMath::CircularNumF(SplineLen, CurrentRouteDistance + DeltaDist);

	CurrentRouteDistance = NextDist;
}

FVector AOmniVehicleBus::GetCurrentRouteLocation() const
{
	const AOmniLineBusRoute* BusLine = OwnerBusRoute.Get();
	if (BusLine == nullptr)
		return FVector::Zero();

	return BusLine->GetRouteSpline()->GetLocationAtDistanceAlongSpline(CurrentRouteDistance, ESplineCoordinateSpace::World);
}

FRotator AOmniVehicleBus::GetLookAtDeltaRotation(const FVector InTargetPos) const
{
	const FVector  ActLoc    = GetActorLocation();
	const FRotator ActRot    = OmniMath::YawRotator(GetActorRotation());
	const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(ActLoc, InTargetPos);

	FRotator DeltaRot = FRotator::ZeroRotator;
	DeltaRot.Yaw      = FMath::Clamp(UKismetMathLibrary::NormalizedDeltaRotator(TargetRot, ActRot).Yaw, -90.f, 90.f);

#if WITH_EDITOR
	if (GetActorRotation().Pitch != 0.0 || GetActorRotation().Roll != 0.0)
		OB_LOG("error bus rot - %s/%s : %s", *GetName(), *GetActorLabel(), *GetActorRotation().ToString())
#endif

	return DeltaRot;
}

void AOmniVehicleBus::SetBusSpeed(const FVector InTargetPos)
{
	const float LengthToTarget = (GetActorLocation() - InTargetPos).Size2D();
	const float DistanceRate   = LengthToTarget / SteeringDistance;

	// 거리 비율에 따른 감속
	float SpeedFactor = 1.0f;

	if (DistanceRate > 1.05f)
		SpeedFactor = FMath::Clamp(DistanceRate, 1.0f, 1.1f);
	else if (DistanceRate < 0.95f)
		SpeedFactor = DistanceRate / 2.0f;
	else if (DistanceRate < 0.1f)
		SpeedFactor = 0.0f;


	SetDriveMaxSpeed(FMath::Clamp(NowRouteSpeed * SpeedFactor, 0.0f, CarSpec.MaxSpeed));
}

USplineComponent* AOmniVehicleBus::GetOwnerRouteSpline() const
{
	const AOmniLineBusRoute* OwnerBusLine = OwnerBusRoute.Get();
	
	return OwnerBusLine ? OwnerBusLine->GetRouteSpline() : nullptr;
}

float AOmniVehicleBus::GetOwnerRouteLength() const
{
	const USplineComponent* Spline = GetOwnerRouteSpline();

	return Spline ? Spline->GetSplineLength() : 0.0f;
}

FBusStopDistance AOmniVehicleBus::GetThisStopDist() const
{
	const AOmniLineBusRoute* OwnerBusLine = OwnerBusRoute.Get();

	return OwnerBusLine ? OwnerBusLine->GetBusStopDist(ThisStopIndex) : FBusStopDistance();
}

void AOmniVehicleBus::SetDriveMaxSpeed(const float InMaxSpeed /*= 1200.0*/)
{
	if (OB_IS_VALID(PawnMovement))
		PawnMovement->MaxSpeed = InMaxSpeed;
}
