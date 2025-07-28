// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniPassenger.h"

#include "OmniAsync.h"
#include "OmnibusGameInstance.h"
#include "OmnibusPlayData.h"
#include "OmnibusRoadManager.h"
#include "OmnibusTypes.h"
#include "OmniPlayMainUI.h"
#include "OmniStationBusStop.h"
#include "OmniTimeManager.h"
#include "OmniVehicleBus.h"
#include "PathVisualizator.h"

#include "UtlLog.h"
#include "UTLStatics.h"

#include <queue>

AOmniPassenger::AOmniPassenger()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PassengerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PassengerMesh"));
	PassengerMesh->SetupAttachment(RootComponent);
	// PassengerMesh->SetVisibility(false, false);
	PassengerMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	InputMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InputMesh"));
	InputMeshComponent->SetupAttachment(RootComponent);
	InputMeshComponent->SetCollisionProfileName(EOmniCollisionProfile::LineTraceOnly);
	InputMeshComponent->SetVisibility(false, false);
	InputMeshComponent->SetHiddenInGame(true);

	HomeSector = FSectorInfo();
	DestSectorList.SetNum(EnumToInt(EDay::Size));

	PathQueryID = INVALID_QUERY_ID;

	PathListIdx = INDEX_NONE;
	StepListIdx = INDEX_NONE;

	CurrentJourneyDirection = EJourneyDirection::HomeToDest;
	CurrentLocationInfo     = FPassengerLocationInfo();

	MyLastRouteMapVersion = 0;
}

void AOmniPassenger::BeginPlay()
{
	Super::BeginPlay();

	InputMeshComponent->OnBeginCursorOver.AddDynamic(this, &AOmniPassenger::BeginCursorOver);
	InputMeshComponent->OnEndCursorOver.AddDynamic(this, &AOmniPassenger::EndCursorOver);

	MyTransferRuleList = GetOmnibusPlayData()->GetTransferRuleList();
	UT_LIGHT_CHECK(MyTransferRuleList.IsEmpty() == false, "TransferRuleList is empty. Check PlayData")

	SetJourneyState(EJourneyState::None);
}

void AOmniPassenger::PostBeginPlay()
{
	Super::PostBeginPlay();
	// 레벨에 미리 스폰되어져 있는 경우 필요한 로직.

	AOmnibusRoadManager* RoadManager = GetOmnibusRoadManager();
	UT_IF(RoadManager == nullptr)
		return;

	RoadManager->AddPassenger(this);

	// 레벨에 미리 깔려있는 경우, 등록 후 자동 시작.
	FUtlTime::SimpleTimer(this, 2.0, [ThisWeak = TWeakObjectPtr<AOmniPassenger>(this)]()
	{
		AOmniPassenger* Passenger = ThisWeak.Get();
		if (Passenger == nullptr)
			return;

		auto [HomeInfo, DestInfoList] = Passenger->GetOmnibusRoadManager()->GetHomeAndDests();
		Passenger->InitStartJourney(HomeInfo, DestInfoList);
	});
}

void AOmniPassenger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AbortPathFinding();

	Super::EndPlay(EndPlayReason);
}

void AOmniPassenger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmniPassenger::BeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	// 경로 없음. todo: 경로가 없음을 나타내는 UI가 필요함.
	if (TransferPathList.IsEmpty())
		return;

	// 경로 시각화.
	AOmnibusRoadManager* const RoadManager = GetOmnibusRoadManager();
	if (RoadManager)
	{
		PathVisualizator = RoadManager->VisualizePath(TransferPathList[0]);
	}
}

void AOmniPassenger::EndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	if (TransferPathList.IsEmpty())
		return;

	const APathVisualizator* Visual = PathVisualizator.Get();
	if (Visual && Visual->IsSamePath(TransferPathList[0]))
		Visual->SetPathVisibility(false);
}

void AOmniPassenger::DisablePassenger()
{
	SetActorTickEnabled(false);
	PassengerMesh->SetVisibility(false, false);
}

void AOmniPassenger::EnablePassenger()
{
	// SetActorTickEnabled(true);
	PassengerMesh->SetVisibility(GetOmnibusRoadManager()->IsPassengerVisible(), false);
}

void AOmniPassenger::SetMeshVisibility(const bool InVisibility)
{
	PassengerMesh->SetVisibility(InVisibility, false);
}

void AOmniPassenger::SetMaterialColor(const FLinearColor& InLinearColor)
{
	UMaterialInstanceDynamic* NewMID = PassengerMesh->CreateDynamicMaterialInstance(0, PassengerMesh->GetMaterial(0));
	if (NewMID)
	{
		NewMID->SetVectorParameterValue(TEXT("Color"), InLinearColor);
		PassengerMesh->SetMaterial(0, NewMID);
	}
}

void AOmniPassenger::SetJourneyState(const EJourneyState InJourneyState)
{
	UOmniPlayMainUI::OnExchangePassengerCount.Broadcast(CurrentJourneyState, InJourneyState);

	CurrentJourneyState = InJourneyState;
	switch (InJourneyState)
	{
		case EJourneyState::None:
			SetMaterialColor(FOmniColor::Gray);
			break;
		case EJourneyState::FindingPath:
			SetMaterialColor(FOmniColor::Yellow);
			break;
		case EJourneyState::NoPath:
			SetMaterialColor(FOmniColor::Red);
			break;
		case EJourneyState::Traveling:
			SetMaterialColor(FOmniColor::Green);
			break;
		case EJourneyState::ArrivalCompleted:
			SetMaterialColor(FOmniColor::Blue);
			break;
		case EJourneyState::Size:
			break;
	}
}

void AOmniPassenger::InitStartJourney(const FSectorInfo& InHomeInfo, const TArray<FSectorInfo>& InDestInfoList)
{
	HomeSector              = InHomeInfo;
	DestSectorList          = InDestInfoList;
	CurrentJourneyDirection = EJourneyDirection::HomeToDest;
	CurrentJourneyStartDay  = GetOmniTimeManager()->GetTodayInLevel();

	CurrentLocationInfo.EntrySubSector(HomeSector);
	const FTransform SectorTransform = FTransform(HomeSector.GetBlockSubSector()->GetWorldLocation());
	SetTransformAndOffsetNoScale(SectorTransform);

	FindPathAndStartJourney();
}

void AOmniPassenger::RetryJourney()
{
	switch (CurrentLocationInfo.GetCurrentState())
	{
		// case FPassengerLocationInfo::EState::None: break;
		// case FPassengerLocationInfo::EState::Disable: break;
		case FPassengerLocationInfo::EState::SubSector:
			FindPathAndStartJourney();
			break;
		case FPassengerLocationInfo::EState::BusStop:
			FindPathAndStartJourneyStopToSector(CurrentLocationInfo.GetBusStop(), GetTargetSectorInfo());
			break;
		// case FPassengerLocationInfo::EState::Bus: break;
		// case FPassengerLocationInfo::EState::Sidewalk: break;
		default:
			return;
	}
}

void AOmniPassenger::RetryJourneyFromStop()
{
	if (CurrentLocationInfo.GetCurrentState() == FPassengerLocationInfo::EState::BusStop)
		RetryJourney();
}

void AOmniPassenger::RetryJourneyFromSector()
{
	if (CurrentLocationInfo.GetCurrentState() == FPassengerLocationInfo::EState::SubSector)
		RetryJourney();
}

void AOmniPassenger::FindPathAndStartJourney()
{
	FindPathAndStartJourneyBetweenSectors(GetStartSectorInfo(), GetTargetSectorInfo());
}

void AOmniPassenger::FindPathAndStartJourneyBetweenSectors(const FSectorInfo& InStartInfo, const FSectorInfo& InEndInfo)
{
	if (InStartInfo.IsValid() == false || InEndInfo.IsValid() == false)
		return;

	TArray<TWeakObjectPtr<AOmniStationBusStop>> StartStopList = InStartInfo.GetSectorBusStop()->GetNeighborStops();
	TArray<TWeakObjectPtr<AOmniStationBusStop>> EndStopList   = InEndInfo.GetSectorBusStop()->GetNeighborStops();

	FindPathAsync(StartStopList, EndStopList, MyTransferRuleList, FPathFindingQueryDelegate::CreateUObject(this, &AOmniPassenger::OnGameThreadBetweenSectors));
}

void AOmniPassenger::FindPathAndStartJourneyStopToSector(AOmniStationBusStop* InStartStop, const FSectorInfo& InEndInfo)
{
	if (IsValid(InStartStop) == false || InEndInfo.IsValid() == false)
		return;

	TArray<TWeakObjectPtr<AOmniStationBusStop>> EndStopList = InEndInfo.GetSectorBusStop()->GetNeighborStops();

	FindPathAsync({InStartStop}, EndStopList, MyTransferRuleList, FPathFindingQueryDelegate::CreateUObject(this, &AOmniPassenger::OnGameThreadStopToSector));
}

void AOmniPassenger::FindPathAsync(const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InStartList, const TArray<TWeakObjectPtr<AOmniStationBusStop>>& InEndList
                                 , const TArray<FTransferRule>& InTransferRuleList, const FPathFindingQueryDelegate& InGameThreadDelegate)
{
	SetJourneyState(EJourneyState::FindingPath);
	TransferPathList.Reset();
	UOmniPathFinder* PathFinder = GetGameInstance<UOmnibusGameInstance>()->GetOmniPathFinder();

	const TQueryID PathFindingQueryID = PathFinder->FindTransferPathAsync({InStartList, InEndList, InTransferRuleList, InGameThreadDelegate});

	UpdatePathQueryID(PathFindingQueryID);
}

void AOmniPassenger::OnGameThreadBetweenSectors(TQueryID InQueryID, TArray<FTransferPath>&& InTransferPath)
{
	bool bUpdateSuccess = UpdateTransferPath(InQueryID, MoveTemp(InTransferPath));
	if (bUpdateSuccess)
	{
		GoFromSector();
	}
}

void AOmniPassenger::OnGameThreadStopToSector(TQueryID InQueryID, TArray<FTransferPath>&& InTransferPath)
{
	bool bUpdateSuccess = UpdateTransferPath(InQueryID, MoveTemp(InTransferPath));
	if (bUpdateSuccess)
	{
		GoFromStop();
	}
}

bool AOmniPassenger::UpdateTransferPath(const TQueryID InQueryID, TArray<FTransferPath>&& InTransferPath)
{
	bool bQueryResult = false;

	// 경로 없음
	if (InTransferPath.IsEmpty())
	{
		bQueryResult = false;
		SetJourneyState(EJourneyState::NoPath);
		GetOmnibusRoadManager()->AddNoTransferPathPassenger(this);
	}
	else if (PathQueryID != InQueryID)
	{
		// 길찾기 재시도 중
		bQueryResult = false;
	}
	else
	{
		bQueryResult = true;
		SetJourneyState(EJourneyState::Traveling);
		TransferPathList = MoveTemp(InTransferPath);
	}

	// 길찾기 완료했으니 초기화.
	PathQueryID = INVALID_QUERY_ID;

	// 노선도 버전 업데이트
	MyLastRouteMapVersion = GetOmnibusRoadManager()->GetBusRouteMapVersion();

	return bQueryResult;
}

void AOmniPassenger::GoFromSector()
{
	PathListIdx = 0;
	StepListIdx = 0;

	FTransferStep* FirstStep = GetCurrentTransferStep();
	if (FirstStep == nullptr)
		return;

	AOmniStationBusStop* FirstStop = FirstStep->BusStop.Get();
	if (FirstStop == nullptr)
		return;

	// 가장 짧은 경로에 있는 버스 정류장과 동일한 경로만 남기고 제거
	TArray<FTransferPath> TempPathList;
	for (FTransferPath& Path : TransferPathList)
	{
		if (Path.TransferStepList[0].BusStop == FirstStop)
			TempPathList.Emplace(Path);
	}
	Swap(TransferPathList, TempPathList);

	CurrentLocationInfo.EntryBusStop(FirstStop);
	const FTransform WaitingZone = FirstStop->WaitAtStopLine(this);
	SetTransformAndOffsetNoScale(WaitingZone);
}

void AOmniPassenger::GoFromStop()
{
	PathListIdx = 0;
	StepListIdx = 0;
	// 이미 정류장에 있으므로 이동 로직 생략.
	// SetTransformAndOffsetNoScale(WaitingZone);
}

int32 AOmniPassenger::GetPathIdxByThisBus(const AOmniVehicleBus* InThisBus)
{
	if (IsValid(InThisBus) == false)
		return INDEX_NONE;

	// 길찾기 다시 시작해서 비어있는 경우.
	if (TransferPathList.IsEmpty())
		return INDEX_NONE;

	// 경로가 여러개인 경우 고려.
	for (int PathIdx = 0; PathIdx < TransferPathList.Num(); ++PathIdx)
	{
		const FTransferStep* MyStep = GetTransferStep(PathIdx, StepListIdx);

		UT_IF(MyStep == nullptr)
			continue;

		if (InThisBus->HasSameTransStep(*MyStep))
		{
			return PathIdx;
		}
	}

	return INDEX_NONE;
}

bool AOmniPassenger::IsEntryToThisBus(const AOmniVehicleBus* InThisBus)
{
	return GetPathIdxByThisBus(InThisBus) >= 0;
}

bool AOmniPassenger::IsExitToThisBusStop(const AOmniStationBusStop* InThisBusStop)
{
	if (IsValid(InThisBusStop) == false)
		return false;

	return GetNextStop() == InThisBusStop;
}

void AOmniPassenger::DoEntryToBus(AOmniVehicleBus* InThisBus, const float InEntryDelay)
{
	// 최초 탑승 시 나머지 경로 정리
	if (TransferPathList.Num() > 1)
	{
		const int32 PathIdx = GetPathIdxByThisBus(InThisBus);
		UT_IF (PathIdx == INDEX_NONE || TransferPathList.IsValidIndex(PathIdx) == false)
		{
			return;
		}
		TArray<FTransferPath> TempPathList = {TransferPathList[PathIdx]};
		Swap(TransferPathList, TempPathList);
	}

	// 버스 탑승 중
	// 탑승 시작 순간부터 위치를 버스로 변경
	// 탑승 중에 NotifyUpdateBusRoute를 통한 경로 재탐색 영향을 받지 않게 하기 위함
	CurrentLocationInfo.EntryBus(InThisBus);
	const FTransform BusTransform = InThisBus->EntryToBus(this);

	FTimerHandle EntryToBusHandle;
	GetWorldTimerManager().SetTimer(EntryToBusHandle, [ThisPassengerWeak = TWeakObjectPtr<AOmniPassenger>(this), InThisBus, BusTransform]()
	{
		if (AOmniPassenger* Passenger = ThisPassengerWeak.Get())
		{
			Passenger->AttachToActor(InThisBus, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			Passenger->SetTransformAndOffsetNoScale(BusTransform);
		}
	}, InEntryDelay, false);
}

void AOmniPassenger::DoExitToStop(AOmniStationBusStop* InThisBusStop, const float InExitDelay)
{
	FTimerHandle ExitToStopHandle;
	GetWorldTimerManager().SetTimer(ExitToStopHandle, [ThisPassengerWeak = TWeakObjectPtr<AOmniPassenger>(this), InThisBusStop]()
	{
		if (AOmniPassenger* Passenger = ThisPassengerWeak.Get())
		{
			Passenger->DoExitToStopImpl(InThisBusStop);
		}
	}, InExitDelay, false);
}

void AOmniPassenger::DoExitToStopImpl(AOmniStationBusStop* InThisBusStop)
{
	UT_IF(IsExitToThisBusStop(InThisBusStop) == false)
	{
		return;
	}

	StepListIdx += 1;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	const FTransferStep* ThisStep = GetCurrentTransferStep();
	if (ThisStep && ThisStep->IsLastStep())
	{
		// 목적지 도착 완료.
		ArriveDestination();
		return;
	}

	CurrentLocationInfo.EntryBusStop(InThisBusStop);
	const FTransform WaitingZone = InThisBusStop->WaitAtStopLine(this);
	SetTransformAndOffsetNoScale(WaitingZone);

	// 노선도 버전이 업데이트 됐다면, 현재 정류장에서부터 다시 경로를 찾습니다.
	if (IsRouteMapUpdated())
	{
		FindPathAndStartJourneyStopToSector(InThisBusStop, GetTargetSectorInfo());
	}
}

void AOmniPassenger::ArriveDestination()
{
	PathListIdx = INDEX_NONE;
	StepListIdx = INDEX_NONE;

	TransferPathList.Empty();
	FSubSector* StartSector  = GetStartSectorInfo().GetBlockSubSector();
	FSubSector* TargetSector = GetTargetSectorInfo().GetBlockSubSector(); //DestSectorList[EnumToInt(EDay::Normal)].GetBlockSubSector();

	StartSector->RemoveVisitor(this);
	TargetSector->AddVisitor(this);

	CurrentLocationInfo.EntrySubSector(GetTargetSectorInfo());
	const FVector Location = TargetSector->GetWorldLocation();
	SetTransformAndOffsetNoScale(FTransform(Location));

	// 방향전환
	SwitchJourneyDirection();

	SetJourneyState(EJourneyState::ArrivalCompleted);

	const float IntervalTime = GetOmniTimeManager()->GetJourneyRepeatIntervalTime();
	GetWorldTimerManager().SetTimer(JourneyRepeatHandle, FTimerDelegate::CreateUObject(this, &AOmniPassenger::FindPathAndStartJourney), IntervalTime, false);
}

FTransferStep* AOmniPassenger::GetTransferStep(const int32 InPathListIdx, const int32 InStepListIdx)
{
	if (TransferPathList.IsValidIndex(InPathListIdx) == false
		|| TransferPathList[InPathListIdx].TransferStepList.IsValidIndex(InStepListIdx) == false)
	{
		return nullptr;
	}

	return &TransferPathList[InPathListIdx].TransferStepList[InStepListIdx];
}

FTransferStep* AOmniPassenger::GetCurrentTransferStep()
{
	return GetTransferStep(PathListIdx, StepListIdx);
}

AOmniStationBusStop* AOmniPassenger::GetNextStop()
{
	// 다음 승차 정류장이 곧 하차 정류장. 버스에 탑승하는 동안, 노선이 변경될 수 있으므로 다음 스탭을 사용.
	const FTransferStep* NextStep = GetTransferStep(PathListIdx, StepListIdx + 1);

	return NextStep ? NextStep->GetStartBusStop() : nullptr;
}

void AOmniPassenger::SetTransformAndOffsetNoScale(FTransform InTransform, const float InZ_Offset)
{
	InTransform.SetScale3D(GetActorScale());
	InTransform.AddToTranslation(FVector(0.0, 0.0, InZ_Offset));

	SetActorTransform(InTransform);
}

void AOmniPassenger::UpdatePathQueryID(const TQueryID InPathQueryID)
{
	// 기존 쿼리 취소.
	if (PathQueryID != INVALID_QUERY_ID)
	{
		UOmniPathFinder* PathFinder = GetGameInstance<UOmnibusGameInstance>()->GetOmniPathFinder();
		PathFinder->AbortPathFinding(PathQueryID);
	}
	PathQueryID = InPathQueryID;
}

bool AOmniPassenger::AbortPathFinding()
{
	// PathQueryID는 쿼리가 완료되면, INVALID_QUERY_ID으로 초기화됩니다.
	// 따라서, PathQueryID가 INVALID_QUERY_ID이라면 취소하기 전에 요청 결과가 이미 적용되었을 수 있습니다.
	if (PathQueryID == INVALID_QUERY_ID)
		return false;

	UpdatePathQueryID(INVALID_QUERY_ID);
	return true;
}

void AOmniPassenger::SwitchJourneyDirection()
{
	if (CurrentJourneyDirection == EJourneyDirection::HomeToDest)
	{
		CurrentJourneyDirection = EJourneyDirection::DestToHome;
	}
	else
	{
		CurrentJourneyDirection = EJourneyDirection::HomeToDest;
	}
}

FSectorInfo AOmniPassenger::GetStartSectorInfo()
{
	if (CurrentJourneyDirection == EJourneyDirection::HomeToDest)
	{
		return HomeSector;
	}

	return DestSectorList[EnumToInt(CurrentJourneyStartDay)];
}

FSectorInfo AOmniPassenger::GetTargetSectorInfo()
{
	if (CurrentJourneyDirection == EJourneyDirection::HomeToDest)
	{
		return DestSectorList[EnumToInt(CurrentJourneyStartDay)];
	}

	return HomeSector;
}

bool AOmniPassenger::IsRouteMapUpdated() const
{
	return MyLastRouteMapVersion != GetOmnibusRoadManager()->GetBusRouteMapVersion();
}
