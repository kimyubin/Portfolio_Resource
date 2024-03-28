// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniVehicleBus.h"

#include "NavigationSystem.h"
#include "OmniAICtrlBus.h"
#include "Omnibus.h"
#include "OmnibusTypes.h"
#include "OmnibusUtilities.h"
#include "OmniLaneApproachCollision.h"
#include "OmniRoadDefaultTwoLane.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "Kismet/GameplayStatics.h"
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
	PawnMovement->TurningBoost      = 16.0;
	PawnMovement->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::Z);


	BusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BusMesh"));
	BusMesh->SetupAttachment(RootComponent);
	BusMesh->SetCanEverAffectNavigation(false);

	ForwardSensingBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Sensor"));
	ForwardSensingBoxComponent->SetupAttachment(RootComponent);
	ForwardSensingBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AOmniVehicleBus::AOmniVehicleBus::BeginOverlapSensor);
	
	CurrentRouteRoadIdx = 1;
	
	AIControllerClass = AOmniAICtrlBus::StaticClass();
	AutoPossessAI     = EAutoPossessAI::PlacedInWorldOrSpawned;

	SteeringDistance  = 300.0;
}

void AOmniVehicleBus::BeginPlay()
{
	Super::BeginPlay();

	GenerateRouteRoad();
}

void AOmniVehicleBus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DriveToDestination();
}

void AOmniVehicleBus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AOmniVehicleBus::DriveToDestination()
{
	// 액터에 새로운 회전값 적용
	AddActorLocalRotation(GetRotationFromOmniRoadSpline());
	AddMovementInput(GetActorForwardVector());
}

FRotator AOmniVehicleBus::GetRotationFromOmniRoadSpline()
{
	const USplineComponent* SplineComp = GetTargetLane();

	const FVector  ActLoc = GetActorLocation();
	const FRotator ActRot = GetActorRotation();

	// 가장 가까운 스플라인의 진행 방향(접선)만 가져와서, 현재 위치에서 바라본 곳을 가상의 목표로 정함..
	FVector ClosestSplineTangent = SplineComp->FindTangentClosestToWorldLocation(ActLoc, ESplineCoordinateSpace::World);
	ClosestSplineTangent.Normalize();
	ClosestSplineTangent *= SteeringDistance;
	ClosestSplineTangent += ActLoc;

	// 액터 약간 앞에 있는 가상 목표에서 가장 가까운 스플라인 위치.
	const FVector  Target    = SplineComp->FindLocationClosestToWorldLocation(ClosestSplineTangent, ESplineCoordinateSpace::World);
	const FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(ActLoc, Target);

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(TargetRot, ActRot);
	DeltaRot.Yaw      = FMath::Clamp(DeltaRot.Yaw, -90.f, 90.f);
	DeltaRot.Roll     = ActRot.Roll;
	DeltaRot.Pitch    = ActRot.Pitch;

	return DeltaRot;
}


void AOmniVehicleBus::BeginOverlapSensor(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->IsA(UOmniLaneApproachCollision::StaticClass()))
	{
		const UOmniLaneApproachCollision* LaneApproach = Cast<UOmniLaneApproachCollision>(OtherComp);
		USplineComponent* NextLane = LaneApproach->GetNextSplineByRoad(GetNextRouteRoad());

		// 다음 레인을 찾지 못했다면, 중복 Overlap 발생한 것이기 때문에 넘어감. 
		if (OB_IS_VALID(NextLane) == true)
		{
			CurrentLane = NextLane;
			++CurrentRouteRoadIdx;
		}
	}
}

AOmniRoad* AOmniVehicleBus::GetNearestOmniRoad() const
{
	TArray<AActor*> OmniRoads;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOmniRoad::StaticClass(), OmniRoads);

	AOmniRoad* NearRoad       = nullptr;
	float DistanceFromNearest = TNumericLimits<float>::Max();
	const FVector ActLoc      = GetActorLocation();
	for (AActor* ActorToCheck : OmniRoads)
	{
		if (IsValid(ActorToCheck) == false)
			continue;

		AOmniRoad* Road     = Cast<AOmniRoad>(ActorToCheck);
		const uint8 LaneNum = Road->GetLaneSplineNum();
		for (int idx = 0; idx < LaneNum; ++idx)
		{
			const USplineComponent* Lane = Road->GetLaneSpline(idx);
			FVector ClosestSplineLoc     = Lane->FindLocationClosestToWorldLocation(ActLoc, ESplineCoordinateSpace::World);
			const float DistanceFromLane = (ActLoc - ClosestSplineLoc).SizeSquared();
			if (DistanceFromLane < DistanceFromNearest)
			{
				NearRoad            = Road;
				DistanceFromNearest = DistanceFromLane;
			}
		}
	}
	
	return NearRoad;
}

USplineComponent* AOmniVehicleBus::GetTargetLane()
{
	if(CurrentLane.IsValid() == false)
	{
		OB_LOG("CurrentLane init")
		CurrentLane = BusRouteRoads[0]->GetLaneSpline(0);
	}
	return CurrentLane.Get();
}

AOmniRoad* AOmniVehicleBus::GetNextRouteRoad()
{
	const uint32 MaxRoute = BusRouteRoads.Num() - 1;
	const uint32 RoadIdx  = OmniMath::CircularNum(MaxRoute, CurrentRouteRoadIdx + 1);

	return BusRouteRoads[RoadIdx].Get();
}

void AOmniVehicleBus::GenerateRouteRoad()
{
	AOmniRoad* const NearRoad   = GetNearestOmniRoad();
	AOmniRoad* const SecondRoad = NearRoad->GetRandomConnectedRoad(nullptr);
	BusRouteRoads = {NearRoad, SecondRoad};

	constexpr int LoopLimit = 1000;
	for (int i = 0; i < LoopLimit; ++i)
	{
		AOmniRoad* LastRoad = BusRouteRoads.Last().Get();
		AOmniRoad* NextRoad = LastRoad->GetRandomConnectedRoad(BusRouteRoads.Last(1).Get());

		if (OB_IS_VALID(NextRoad) == false)
			continue;

		// 최초 출발지에 도착하면 탈출(경로를 루프로 구성함)
		if (LastRoad != SecondRoad && NextRoad == NearRoad)
			break;

		BusRouteRoads.Push(NextRoad);
	}
}

void AOmniVehicleBus::SetDriveMaxSpeed(const double InMaxSpeed /*= 1200.0*/)
{
	if(OB_IS_VALID(PawnMovement))
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

void AOmniVehicleBus::MoveToTarget(const AActor* DestActor)
{
	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	AOmniAICtrlBus*      AIController     = Cast<AOmniAICtrlBus>(Controller);
	FNavPathSharedPtr    NavPath;
	if (OB_IS_VALID(AIController))
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(DestActor);
		MoveRequest.SetAcceptanceRadius(5.f);

		FPathFindingQuery Query;
		Query.StartLocation = GetActorLocation();
		Query.EndLocation   = DestActor->GetActorLocation();
		Query.NavData       = NavigationSystem->GetNavDataForProps(GetNavAgentPropertiesRef());
		
		FPathFindingResult PathFindingResult = NavigationSystem->FindPathSync(Query);
		if (PathFindingResult.IsSuccessful())
		{
			TArray<FNavPathPoint> PathPoints = PathFindingResult.Path->GetPathPoints();
			for (const FNavPathPoint& Point : PathPoints)
			{
				OB_LOG_STR(Point.Location.ToString())
				UKismetSystemLibrary::DrawDebugSphere(this, Point.Location, 200.f, 8, FLinearColor::Red, 100, 10);
			}
		}

		AIController->FindPathForMoveRequest(MoveRequest, Query, NavPath);
	}
	
	if (NavPath.IsValid())
	{
		TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		for (const FNavPathPoint& Point : PathPoints)
		{
			OB_LOG_STR(Point.Location.ToString())
			UKismetSystemLibrary::DrawDebugSphere(this, Point.Location, 200.f, 8, FLinearColor::Red, 100, 10);
		}
	}
}
