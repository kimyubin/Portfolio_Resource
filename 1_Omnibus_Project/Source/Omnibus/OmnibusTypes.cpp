// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusTypes.h"

#include "OmniCityBlock.h"
#include "OmniLineBusRoute.h"
#include "OmniPassenger.h"
#include "OmniRoad.h"
#include "OmniStationBusStop.h"
#include "OmniVehicleBus.h"
#include "Components/SplineComponent.h"

#include "UtlLog.h"
#include "UTLStatics.h"

const FName EOmniComponentTag::StartPoint_RoadSpline("StartPoint_RoadSpline");
const FName EOmniComponentTag::EndPoint_RoadSpline("EndPoint_RoadSpline");
const FName EOmniComponentTag::StartPoint_Lane("StartPoint_Lane");
const FName EOmniComponentTag::EndPoint_Lane("EndPoint_Lane");

const FName EOmniCollisionProfile::RoadConnectorOverlap("RoadConnectorOverlap");
const FName EOmniCollisionProfile::SysDetectorOverlap("SysDetectorOverlap");
const FName EOmniCollisionProfile::BusDetectorOverlap("BusDetectorOverlap");
const FName EOmniCollisionProfile::LineTraceOnly("LineTraceOnly");

const ECollisionChannel EOmniCollisionChannel::OverlapOnlyTrace = ECollisionChannel::ECC_GameTraceChannel4;


//~=============================================================================
// FCarSpecRow
FCarSpecRow::FCarSpecRow()
	: CarType(ECarType::None)
	, PassengerCapacity(0)
	, ExitSpeed(0.0f)
	, MaxSpeedFactor(0.0f)
	, AccelerationFactor(0.0f)
	, StopSpacing(0.0f) {}

void FCarSpecRow::OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems)
{
	FTableRowBase::OnPostDataImport(InDataTable, InRowName, OutCollectedImportProblems);
}


//~=============================================================================
// FCarSpec
FCarSpec::FCarSpec()
	: CarType(ECarType::None)
	, PassengerCapacity(0)
	, MaxSpeed(0.0f)
	, Acceleration(0.0f)
	, StopSpacing(0.0f)
	, ExitSpeed(0.0f) {}

FCarSpec::FCarSpec(const ECarType InCarType
                 , const int16 InPassengerCapacity
                 , const float InExitSpeed
                 , const float InMaxSpeed
                 , const float InAcceleration
                 , const float InStopSpacing)
	: CarType(InCarType)
	, PassengerCapacity(InPassengerCapacity)
	, MaxSpeed(InMaxSpeed)
	, Acceleration(InAcceleration)
	, StopSpacing(InStopSpacing)
	, ExitSpeed(InExitSpeed) {}

FCarSpec::FCarSpec(const FCarSpecRow& InSpecRowData, const float InBaseBusSpeed, const float InBaseBusAcceleration)
	: CarType(InSpecRowData.CarType)
	, PassengerCapacity(InSpecRowData.PassengerCapacity)
	, MaxSpeed(InSpecRowData.MaxSpeedFactor * InBaseBusSpeed)
	, Acceleration(InSpecRowData.AccelerationFactor * InBaseBusAcceleration)
	, StopSpacing(InSpecRowData.StopSpacing)
	, ExitSpeed(InSpecRowData.ExitSpeed) {}


//~=============================================================================
// FOmniColor
const FLinearColor FOmniColor::Transparent = FLinearColor::Transparent;
const FLinearColor FOmniColor::White  = FLinearColor::White;
const FLinearColor FOmniColor::Gray   = {0.1f, 0.1f, 0.1f};
const FLinearColor FOmniColor::Black  = FLinearColor::Black;
const FLinearColor FOmniColor::Red    = ConvertRGBToLinear(255, 0, 0);
const FLinearColor FOmniColor::Green  = ConvertRGBToLinear(0, 115, 0);
const FLinearColor FOmniColor::Blue   = ConvertRGBToLinear(0, 0, 138);
const FLinearColor FOmniColor::Yellow = FLinearColor::Yellow;

FLinearColor FOmniColor::GetNextColorByHue(const uint8 ColorSplitNumber, const uint8 CountFactor)
{
	// ColorSplitNumber단계로 색조 분할
	const float SliceInterval = 255.0f / static_cast<float>(ColorSplitNumber);

	static float Hue = (SliceInterval * CountFactor) * 3; // 초기값은 눈에 띄는 색으로
	Hue += (SliceInterval * CountFactor);
	Hue = UtlMath::CircularNumF(255.0f, Hue);
	const uint8 Hue8 = static_cast<uint8>(FMath::RoundHalfToEven(Hue));

	// 배경과 대비를 위해 명도를 약간 낮춤.
	return FLinearColor::MakeFromHSV8(Hue8, 255, 255 * 0.75);
}

FLinearColor FOmniColor::ConvertRGBToLinear(const uint8 R, const uint8 G, const uint8 B, const uint8 A)
{
	return FColor(R, G, B, A).ReinterpretAsLinear();
}

FLinearColor FOmniColor::ConvertHexToLinear(const FString& HexString)
{
	return FColor::FromHex(HexString).ReinterpretAsLinear();
}


//~=============================================================================
// FLaneInfo 
FLaneInfo::FLaneInfo()
	: OmniRoad(nullptr)
	, LaneIdx(INDEX_NONE) {}

FLaneInfo::FLaneInfo(AOmniRoad* InRoad, const int32 InLaneIdx)
	: OmniRoad(InRoad)
	, LaneIdx(InLaneIdx) {}

bool FLaneInfo::operator==(const FLaneInfo& InLaneInfo) const
{
	return ((OmniRoad == InLaneInfo.OmniRoad) && (LaneIdx == InLaneInfo.LaneIdx));
}

USplineComponent* FLaneInfo::GetLaneSpline() const
{
	if (UT_IS_WEAK_PTR_VALID(OmniRoad) == false)
		return nullptr;

	return OmniRoad->GetLaneSpline(LaneIdx);
}

float FLaneInfo::GetLength() const
{
	if (UT_IS_WEAK_PTR_VALID(OmniRoad) == false)
		return FOmniConst::GetBigNumber<float>();

	if(LaneIdx == FLaneInfo::UTURN_LANE)
	{
		const USplineComponent* FstLane = OmniRoad.Get()->GetLaneSpline(0);
		const USplineComponent* SecLane = OmniRoad.Get()->GetLaneSpline(1);
		const FVector FstPoint          = FstLane->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
		const FVector SecPoint          = SecLane->GetLocationAtSplinePoint(SecLane->GetNumberOfSplinePoints(), ESplineCoordinateSpace::Local);

		// 차선 간격을 지름으로 하는 원 둘레의 절반.
		return (FstPoint - SecPoint).Size2D() * UE_HALF_PI;
	}
	return GetLaneSpline()->GetSplineLength();
}

FVector FLaneInfo::GetPointLocation(const int32 InputKey) const
{
	if (UT_IS_WEAK_PTR_VALID(OmniRoad) == false)
		return FVector::Zero();

	return GetLaneSpline()->GetLocationAtSplinePoint(InputKey, ESplineCoordinateSpace::World);
}

FVector FLaneInfo::GetStartPoint() const
{
	return GetPointLocation(0);
}

FVector FLaneInfo::GetEndPoint() const
{
	return GetPointLocation(GetLaneSpline()->GetNumberOfSplinePoints() - 1);
}

FVector2D FLaneInfo::GetPointLocation2D(const int32 InputKey) const
{
	return FVector2D(GetPointLocation(InputKey));
}

FVector2D FLaneInfo::GetStartPoint2D() const
{
	return FVector2D(GetStartPoint());
}

FVector2D FLaneInfo::GetEndPoint2D() const
{
	return FVector2D(GetEndPoint());
}

FLaneInfo FLaneInfo::UTurnInfo()
{
	return {nullptr, FLaneInfo::UTURN_LANE};
}

TArray<FLaneInfo> FLaneInfo::GetNextLaneInfos() const
{
	if (UT_IS_WEAK_PTR_VALID(OmniRoad) == false)
		return {};

	TArray<FLaneInfo> Res;

	AOmniRoad* Road     = OmniRoad.Get();
	AOmniRoad* NextRoad = Road->GetNextRoadByLaneIdx(LaneIdx);
	UT_IF (NextRoad == nullptr)
		return {};

	const int32 NextRoadConnectNum = NextRoad->GetConnectedRoadNum();
	// 다음 도로에서 갈 수 있는 차선들 확인.
	for (int NextIdx = 0; NextIdx < NextRoadConnectNum; ++NextIdx)
	{
		AOmniRoad* NextNextRoad = NextRoad->GetConnectedRoad(NextIdx);
		if (IsValid(NextNextRoad) == false || NextNextRoad == Road)
			continue;

		const int32 NextLaneIdx = NextRoad->FindLaneIdxToNextRoad(Road, NextNextRoad);
		if (NextLaneIdx == INDEX_NONE)
			continue;
		
		Res.Emplace(NextRoad, NextLaneIdx);
	}
	return Res;
}

size_t std::hash<FLaneInfo>::operator()(const FLaneInfo& InLaneInfo) const
{
	// static_assert(sizeof(size_t) == sizeof(uint64), "size_t and uint64 are different size."); // UE5는 32비트 미지원
	// 도로 WeakPtr 해시와 차선 번호로 해싱
	return (static_cast<uint64>(GetTypeHash(InLaneInfo.OmniRoad)) << 32) | static_cast<uint64>(InLaneInfo.LaneIdx);
}


//~=============================================================================
// FTransferStep
FTransferStep::FTransferStep()
	: BusStop(nullptr)
	, BusLine(nullptr)
	, StartBusStopDistanceIdx(INDEX_NONE)
	, EndBusStopDistanceIdx(INDEX_NONE)
	, ExpectedDist(0) {}

FTransferStep::FTransferStep(TWeakObjectPtr<AOmniStationBusStop> InBusStop
                           , TWeakObjectPtr<AOmniLineBusRoute> InBusLine
                           , const int32 InStartStopIdxInBusLine
                           , const int32 InEndStopIdxInBusLine
                           , const float InExpectedDist)
	: BusStop(InBusStop)
	, BusLine(InBusLine)
	, StartBusStopDistanceIdx(InStartStopIdxInBusLine)
	, EndBusStopDistanceIdx(InEndStopIdxInBusLine)
	, ExpectedDist(InExpectedDist) {}

bool FTransferStep::operator==(const FTransferStep& InTransStep) const
{
	return (BusStop == InTransStep.BusStop)
			&& (BusLine == InTransStep.BusLine)
			&& (StartBusStopDistanceIdx == InTransStep.StartBusStopDistanceIdx)
			&& (EndBusStopDistanceIdx == InTransStep.EndBusStopDistanceIdx)
			&& (ExpectedDist == InTransStep.ExpectedDist);
}

FTransferStep FTransferStep::MakeLastStep(TWeakObjectPtr<AOmniStationBusStop> InBusStop)
{
	FTransferStep Result = FTransferStep();
	Result.BusStop       = InBusStop;
	return Result;
}

bool FTransferStep::IsLastStep() const
{
	return BusStop.IsValid()
			&& (BusLine.IsValid() == false || StartBusStopDistanceIdx == INDEX_NONE || EndBusStopDistanceIdx == INDEX_NONE);
}

AOmniStationBusStop* FTransferStep::GetStartBusStop() const
{
	return BusStop.Get();
}

AOmniStationBusStop* FTransferStep::GetEndBusStop() const
{
	AOmniLineBusRoute* BusLinePtr = BusLine.Get();
	if (BusLinePtr == nullptr)
		return nullptr;

	return BusLinePtr->GetBusStopDist(EndBusStopDistanceIdx).BusStop.Get();
}

size_t std::hash<FTransferStep>::operator()(const FTransferStep& InTransferStep) const
{
	uint32 Hash = HashCombine(GetTypeHash(InTransferStep.BusLine), GetTypeHash(InTransferStep.StartBusStopDistanceIdx));
	Hash        = HashCombine(Hash, GetTypeHash(InTransferStep.EndBusStopDistanceIdx));
	Hash        = HashCombine(Hash, GetTypeHash(InTransferStep.ExpectedDist));

	return Hash;
}


//~=============================================================================
// FTransferPath
FTransferPath::FTransferPath()
	: TransferStepList(TArray<FTransferStep>()) {}

FTransferPath::FTransferPath(const TArray<FTransferStep>& InTransferStepList)
	: TransferStepList(InTransferStepList) {}

FTransferPath::FTransferPath(const FTransferPath& Other)
	: TransferStepList(Other.TransferStepList) {}

FTransferPath::FTransferPath(FTransferPath&& Other) noexcept
	: TransferStepList(MoveTemp(Other.TransferStepList)) {}


bool FTransferPath::IsValidPath() const
{
	if (TransferStepList.IsEmpty())
		return false;

	if(TransferStepList.Num() == 1 && TransferStepList[0].IsLastStep())
		return false;

	return true;
}

float FTransferPath::GetTotalPathDistance() const
{
	float Result = 0.0f;
	for (const FTransferStep& Step : TransferStepList)
	{
		Result += Step.ExpectedDist;
	}

	return Result;
}

void FTransferPath::Reset(const uint32 NewSize)
{
	TransferStepList.Reset(NewSize);
}


//~=============================================================================
// FBusStopDistance
FBusStopDistance::FBusStopDistance()
	: BusStop(nullptr)
	, Distance(DISTANCE_NONE) {}

FBusStopDistance::FBusStopDistance(const TWeakObjectPtr<AOmniStationBusStop>& InBusStop, float InDistance)
	: BusStop(InBusStop)
	, Distance(InDistance) {}

bool FBusStopDistance::IsValid() const
{
	return Distance != DISTANCE_NONE && BusStop.IsValid();
}

//~=============================================================================
// FWayPoint
FWayPoint::FWayPoint()
	: BusStop(nullptr)
	, LaneInfoRouteIdx(INDEX_NONE) {}

FWayPoint::FWayPoint(const TWeakObjectPtr<AOmniStationBusStop>& InBusStop, const int32 InLaneInfoRouteIdx)
	: BusStop(InBusStop)
	, LaneInfoRouteIdx(InLaneInfoRouteIdx) {}


//~=============================================================================
// FSubSector
FSubSector::FSubSector()
	: CityBlock(nullptr)
	, BusStop(nullptr)
	, RelativeLocation(0) {}

FSubSector::FSubSector(const TWeakObjectPtr<AOmniCityBlock>& InCityBlock
                     , const TWeakObjectPtr<AOmniStationBusStop>& InBusStop
                     , const FVector& InRelativeLocation)
	: CityBlock(InCityBlock)
	, BusStop(InBusStop)
	, RelativeLocation(InRelativeLocation) {}

FVector FSubSector::GetWorldLocation() const
{
	AOmniCityBlock* Block = CityBlock.Get();
	return Block ? Block->GetActorLocation() + RelativeLocation : FVector::Zero();
}

FVector FSubSector::GetRelativeLocation() const
{
	return RelativeLocation;
}

void FSubSector::AddVisitor(AOmniPassenger* InPassenger)
{
	if (IsValid(InPassenger))
		CurrentVisitorList.Emplace(InPassenger);
}

void FSubSector::RemoveVisitor(AOmniPassenger* InPassenger)
{
	if (IsValid(InPassenger))
		CurrentVisitorList.Remove(TWeakObjectPtr<AOmniPassenger>(InPassenger));
}


//~=============================================================================
// FSectorInfo
FSectorInfo::FSectorInfo()
	: CityBlock(nullptr)
	, SectorIdx(INDEX_NONE) {}

FSectorInfo::FSectorInfo(const TWeakObjectPtr<AOmniCityBlock>& InCityBlock, const int32 InSectorIdx)
	: CityBlock(InCityBlock)
	, SectorIdx(InSectorIdx) {}

bool FSectorInfo::IsValid() const
{
	return CityBlock.IsValid() && SectorIdx != INDEX_NONE;
}

FSubSector* FSectorInfo::GetBlockSubSector() const
{
	if (IsValid() == false)
		return nullptr;

	return CityBlock->GetSubSector(SectorIdx);
}

AOmniStationBusStop* FSectorInfo::GetSectorBusStop() const
{
	if (IsValid() == false)
		return nullptr;

	return CityBlock->GetSubSector(SectorIdx)->BusStop.Get();
}


//~=============================================================================
// FSubSector
FPassengerLocationInfo::EState FPassengerLocationInfo::IndexToEState(const int32 InIdx)
{
	switch (InIdx)
	{
		case 0: return EState::SubSector;
		case 1: return EState::BusStop;
		case 2: return EState::Bus;
		default: ;
	}
	return EState::None;
}

void FPassengerLocationInfo::EntrySubSector(const FSectorInfo& InSectorInfo)
{
	EntryLocation(InSectorInfo);
}

void FPassengerLocationInfo::EntryBusStop(AOmniStationBusStop* InBusStop)
{
	EntryLocation(InBusStop);
}

void FPassengerLocationInfo::EntryBus(AOmniVehicleBus* InBus)
{
	EntryLocation(InBus);
}

void FPassengerLocationInfo::EntryLocation(const FLocationVariant& InLocationVariant)
{
	CurrentState = IndexToEState(InLocationVariant.index());
	LocationInfo = InLocationVariant;
}

FSectorInfo FPassengerLocationInfo::GetSubSector() const
{
	if (ensure(IndexToEState(LocationInfo.index()) == EState::SubSector))
		return std::get<FSectorInfo>(LocationInfo);

	return {};
}

AOmniStationBusStop* FPassengerLocationInfo::GetBusStop() const
{
	if (ensure(IndexToEState(LocationInfo.index()) == EState::BusStop))
		return std::get<TWeakObjectPtr<AOmniStationBusStop>>(LocationInfo).Get();

	return nullptr;
}

AOmniVehicleBus* FPassengerLocationInfo::GetBus() const
{
	if (ensure(IndexToEState(LocationInfo.index()) == EState::Bus))
		return std::get<TWeakObjectPtr<AOmniVehicleBus>>(LocationInfo).Get();

	return nullptr;
}
