// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniVehicleBus.h"

#include "OmniAICtrlBus.h"
#include "Omnibus.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniLineBusRoute.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

AOmniVehicleBus::AOmniVehicleBus()
{
	PrimaryActorTick.bCanEverTick = true;

	SetCanAffectNavigationGeneration(false);
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	PawnMovement = CreateDefaultSubobject<USpectatorPawnMovement>(TEXT("MovementComponent"));
	PawnMovement->UpdatedComponent  = RootComponent;
	PawnMovement->bConstrainToPlane = true;
	PawnMovement->MaxSpeed          = 1200.0;
	PawnMovement->Acceleration      = 1200.0;
	PawnMovement->TurningBoost      = 16.0;
	PawnMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);

	BusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BusMesh"));
	BusMesh->SetupAttachment(RootComponent);
	BusMesh->SetCanEverAffectNavigation(false);

	ForwardSensingBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Sensor"));
	ForwardSensingBoxComponent->SetupAttachment(RootComponent);
	
	AIControllerClass = AOmniAICtrlBus::StaticClass();
	AutoPossessAI     = EAutoPossessAI::PlacedInWorldOrSpawned;

	SteeringDistance     = 150.0;
	MyBusRoute           = nullptr;
	CurrentRouteDistance = 0.0f;
	RouteTargetMoveSpeed = PawnMovement->MaxSpeed;
}

void AOmniVehicleBus::BeginPlay()
{
	Super::BeginPlay();

	// 버스 노선없이 스폰된 경우, 로그 남기고 파괴
	if (OB_IS_WEAK_PTR_VALID(MyBusRoute) == false)
	{
		OB_LOG("The Bus spawned alone without a 'MyBusRoute'.")
		Destroy();
		return;
	}
	
	// 스폰 지역에서 가장 가까운 SplineDistance로 초기화
	const USplineComponent* SplineComp = MyBusRoute->GetRouteSpline();
	const float ClosestInputKey        = SplineComp->FindInputKeyClosestToWorldLocation(GetActorLocation());
	CurrentRouteDistance               = SplineComp->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);
}

void AOmniVehicleBus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DriveToDestination(DeltaTime);
}

void AOmniVehicleBus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOmniVehicleBus::SetupSpawnInit(AOmniLineBusRoute* InOuterRoute)
{
	MyBusRoute = InOuterRoute;
}

void AOmniVehicleBus::DriveToDestination(const float DeltaTime)
{
	// 노선 스플라인 기반 회전값 적용
	const FVector TargetPoint = GetTargetPointFromRouteSpline(DeltaTime);
	//목표를 바라보는 회전 가져오기
	AddActorLocalRotation(GetRotationToTarget(TargetPoint));
	SetBusSpeed(TargetPoint);
	AddMovementInput(GetActorForwardVector());
}

FVector AOmniVehicleBus::GetTargetPointFromRouteSpline(const float DeltaTime)
{	
	if (OB_IS_WEAK_PTR_VALID(MyBusRoute) == false)
		return FVector();
	
	constexpr ESplineCoordinateSpace::Type CoordSpace = ESplineCoordinateSpace::World;

	const USplineComponent* SplineComp = MyBusRoute->GetRouteSpline();

	const float VelocityFactor = DeltaTime * RouteTargetMoveSpeed;
	const float SplineLen      = SplineComp->GetSplineLength();
	CurrentRouteDistance       = OmniMath::CircularNumF(SplineLen, CurrentRouteDistance + VelocityFactor);
	const FVector TargetPoint  = SplineComp->GetLocationAtDistanceAlongSpline(CurrentRouteDistance, CoordSpace);

	UKismetSystemLibrary::DrawDebugSphere(this, FVector(TargetPoint.X, TargetPoint.Y, 10), 12.f, 12, FLinearColor::Red, 1.5f, 5.f);

	return TargetPoint;
}

FRotator AOmniVehicleBus::GetRotationToTarget(const FVector InTargetPos)
{
	const FVector  ActLoc    = GetActorLocation();
	const FRotator ActRot    = GetActorRotation();
	const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(ActLoc, InTargetPos);

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(TargetRot, ActRot);
	DeltaRot.Yaw      = FMath::Clamp(DeltaRot.Yaw, -90.f, 90.f);
	DeltaRot.Roll     = ActRot.Roll;
	DeltaRot.Pitch    = ActRot.Pitch;

	return DeltaRot;
}

void AOmniVehicleBus::SetBusSpeed(const FVector InTargetPos)
{
	const double SquaredLen          = (GetActorLocation() - InTargetPos).SquaredLength();
	const double SquaredSteeringDist = SteeringDistance * SteeringDistance;
	const double DistanceSquaredRate = SquaredLen / SquaredSteeringDist;

	// 거리 비율 110% 초과면 110%로 가속, 90% 미만이면 거리비례로 감속.  
	double SpeedFactor = 1;
	if (DistanceSquaredRate > 1.21)
		SpeedFactor = 1.1;
	else if (DistanceSquaredRate < 0.81)
		SpeedFactor = sqrt(DistanceSquaredRate);
	
	SetDriveMaxSpeed(SpeedFactor * RouteTargetMoveSpeed);
}

void AOmniVehicleBus::SetDriveMaxSpeed(const double InMaxSpeed /*= 1200.0*/)
{
	if (OB_IS_VALID(PawnMovement))
		PawnMovement->MaxSpeed = InMaxSpeed;
}

void AOmniVehicleBus::SetDriveAcceleration(const double InAcceleration /*= 4000.0*/)
{
	if (OB_IS_VALID(PawnMovement))
		PawnMovement->Acceleration = InAcceleration;
}

void AOmniVehicleBus::SetDriveDeceleration(const double InDeceleration /*= 12000.0*/)
{
	if (OB_IS_VALID(PawnMovement))
		PawnMovement->Deceleration = InDeceleration;
}
