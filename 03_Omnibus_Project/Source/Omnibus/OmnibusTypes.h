// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <variant>

#include "CoreMinimal.h"
#include "Templates/TypeHash.h"
#include "OmnibusTypes.generated.h"


class AOmniVehicleBus;
class AOmniPassenger;
class AOmniCityBlock;
class AOmniLineBusRoute;
class AOmniStationBusStop;
class USplineComponent;
class AOmniRoad;


/**
 * enum class를 기본형으로 변환.
 * @tparam E enum class Only
 * @param e 변환 대상 enum
 * @return static_cast<uint8>(e). std::underlying_type_t<E>(e)
 */
template <typename E>
constexpr std::enable_if_t<std::is_enum_v<E>, std::underlying_type_t<E>> EnumToInt(E e) noexcept
{
	return static_cast<std::underlying_type_t<E>>(e);
}

/** 공용 상수*/
struct FOmniConst
{
	/** 프로젝트 단위 길이. 1칸을 의미. 단위 길이 당 월드의 길이. 언리얼 1단위는 1cm와 동일. */
	static constexpr double Unit_Length = 10.0;

	/** 충분히 큰 값. 각 기본 유형별 최대값의 8분의 1. */
	template <typename T>
	static T GetBigNumber()
	{
		static T BigNumber = TNumericLimits<T>::Max() / static_cast<T>(8);
		return BigNumber;
	}
};

UENUM()
enum class EOmniPlayMode : uint8
{
	None,
	Move,
	Edit,
	Build,
};

/** 차량 종류. 차량의 성능 지표를 나타냄. */
UENUM()
enum class ECarType : uint8
{
	None UMETA(DisplayName = "None"),
	Blue_Basic UMETA(DisplayName = "Blue_Basic"),   // 간선용. 중간 속도, 중간 가속, 대량 수송
	Green_Basic UMETA(DisplayName = "Green_Basic"), // 마을용. 약간 낮은 속도, 빠른 가속, 소량 수송
	Red_Basic UMETA(DisplayName = "Red_Basic"),     // 광역용. 높은 속도, 느린 가속, 중간 수송.

	Size UMETA(DisplayName = "Size"),
};

/** 차종별 스펙 테이블 Row를 정의하는 구조체입니다. CSV에서 가져옵니다. */
USTRUCT(BlueprintType)
struct FCarSpecRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCarSpecRow();

	virtual void OnPostDataImport(const UDataTable* InDataTable, const FName InRowName, TArray<FString>& OutCollectedImportProblems) override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ECarType CarType;

	/** 버스 탑승 정원 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int16 PassengerCapacity;

	/**
	 * 하차 속도. 내리는 승객의 시간 간격의 역수.
	 * 10이라면 0.1초 마다 1명 씩 내림.
	 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ExitSpeed;

	/** 최대 속도 배수. 기본 속도의 몇 배인지 나타냅니다. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxSpeedFactor;

	/** 가속도 배수. 기본 가속도의 몇 배인지 나타냅니다. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float AccelerationFactor;

	/** 감속 시작 위치. 감속해야하는 거리 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float StopSpacing;
};

/**
 * 각 차종별 주행 관련 스펙
 */
USTRUCT(BlueprintType)
struct FCarSpec
{
	GENERATED_BODY()
	FCarSpec();

	FCarSpec(const ECarType InCarType, const int16 InPassengerCapacity, const float InExitSpeed, const float InMaxSpeed, const float InAcceleration, const float InStopSpacing);

	explicit FCarSpec(const FCarSpecRow& InSpecRowData, const float InBaseBusSpeed, const float InBaseBusAcceleration);

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ECarType CarType;

	/** 버스 탑승 정원 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int16 PassengerCapacity;

	/** 이동 속도 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxSpeed;

	/** 가속도 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float Acceleration;

	/** 감속 시작 위치. 감속해야하는 거리 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float StopSpacing;

	/**
	 * 하차 속도. 내리는 승객의 시간 간격의 역수.
	 * 10이라면 0.1초 마다 1명 씩 내림.
	 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ExitSpeed;
};


UENUM()
enum class EBusLineType : uint8
{
	None,
	Trunk,       // 간선버스, 메인 노선 정류장, 중~장거리 노선, 블루
	Branch,      // 지선버스, 지선 노선 정류장, 단~중거리 노선, 그린
	Community,   // 마을버스, 마을 노선 정류장, 단거리 노선, 연두
	Circulation, // 순환버스, 순환 노선 정류장, 노랑
	Rapid,       // 광역버스, 광역 노선 정류장, 레드
	Size,
};

UENUM()
enum class EBusStopType : uint8
{
	None,
	Trunk,       // 간선버스, 메인 노선 정류장, 중~장거리 노선, 블루
	Branch,      // 지선버스, 지선 노선 정류장, 단~중거리 노선, 그린
	Community,   // 마을버스, 마을 노선 정류장, 단거리 노선, 연두
	Circulation, // 순환버스, 순환 노선 정류장, 노랑
	Rapid,       // 광역버스, 광역 노선 정류장, 레드
	Size,
};

UENUM()
enum class ERoadConnectorPoint : uint8
{
	None,
	Start,
	End,
	Size,
};

/** 진행방향 */
UENUM()
enum class ERoadDirection : uint8
{
	Left     = 0,
	Straight = 1,
	Right    = 2,
	Size,
};

/** 진입 위치 */
UENUM()
enum class ERoadApproach
{
	Top    = 0,
	Right  = 1,
	Bottom = 2,
	Left   = 3,
	Size,
};

/** 현재 CityBlock의 상태, 종류 */
UENUM()
enum class ECityBlockType : uint8
{
	None,
	/** 재개발중 */
	Redevelopment,
	/** 주거지역 */
	Residence,
	/** 상업지역 */
	Commerce,
	/** 공업지역 */
	Industry,
	/** 관광지 */
	Tour,
	/** 녹지, 공원 */
	GreenSpace,
	Size,
};

/** 길찾기 결과 상태 확인 */
enum class EFindPath : uint8
{
	None,	
	Success,
	Interruption_By_Update,
	Fail_No_Path,
	Fail_Invalid_Stop,
	Fail_Invalid_Line,	
	Size,
};

/** 요일, 주말, 공휴일 등을 구분합니다. */
UENUM()
enum class EDay :uint8
{
	/** 평일 */
	Normal,
	/** 휴일 */
	Holiday,


	Size,
};

/** 게임 태그용 FName 모음*/
struct EOmniComponentTag
{
	static const FName StartPoint_RoadSpline;
	static const FName EndPoint_RoadSpline;
	static const FName StartPoint_Lane;
	static const FName EndPoint_Lane;
};

/** 콜리전 프로필 FName 모음 */
struct EOmniCollisionProfile
{
	/**
	 * 도로 간 연결 전용.
	 * RoadConnector 오버랩 전용. 그외 Ignore.
	 */
	static const FName RoadConnectorOverlap;

	/**
	 * 버스 정류장과 도로 등, 시스템적으로 서로 감지해야하는 콜리전 간 사용.
	 * SysDetector 오버랩 전용. 그외 Ignore. 
	 * 게임 플레이 중 미사용. (정류장에 접근한 버스를 감지하는 기능 등에는 사용 안함.)
	 */
	static const FName SysDetectorOverlap;

	/**
	 * 버스와 버스, 버스와 정류장 등 감지하는 콜리전
	 * 
	 */
	static const FName BusDetectorOverlap;

	/** 커서, 클릭, 터치 등 라인트레이스 감지 전용. UI 오브젝트. */
	static const FName LineTraceOnly;
};

struct EOmniCollisionChannel
{
	/** 오버랩 전용 트레이스 채널 */
	static const ECollisionChannel OverlapOnlyTrace;
};

struct FOmniColor
{
	/**
	 * HSV 색조(Hue)를 N번 분할한 후 순서대로 가져옵니다. 배수를 조절해 색을 건너뛸 수 있습니다.
	 *
	 * @param ColorSplitNumber 분할 횟수. 최대 255까지 나눌 수 있음. 색상 간격의 N/255 만큼 차이나게됨.
	 * @param CountFactor 순번이 늘어나는 배수. 색을 건너뛸 수 있음.
	 * @return N/255 다음 순번의 색
	 */
	static FLinearColor GetNextColorByHue(const uint8 ColorSplitNumber = 32, const uint8 CountFactor = 1);

	/** 8비트 RGBA 색상값을 float 선형 색상으로 변환합니다. */
	static FLinearColor ConvertRGBToLinear(const uint8 R, const uint8 G, const uint8 B, const uint8 A = TNumericLimits<uint8>().Max());

	/**
	 * 16진수 색상을 표현하는 문자열을 선형 색상으로 변환합니다.
	 *
	 * 지원 유형 : RGB, RRGGBB, RRGGBBAA, #RGB, #RRGGBB, #RRGGBBAA
	 */
	static FLinearColor ConvertHexToLinear(const FString& HexString);


	static const FLinearColor Transparent;
	static const FLinearColor White;
	static const FLinearColor Gray;
	static const FLinearColor Black;
	static const FLinearColor Red;
	static const FLinearColor Green;
	static const FLinearColor Blue;
	static const FLinearColor Yellow;
};

/** 도로용 스태틱 메시 보관용 세트 */
USTRUCT(BlueprintType)
struct FRoadStaticMeshSet
{
	GENERATED_BODY()

public:
	FRoadStaticMeshSet() : PlainRoadMesh(nullptr), IntersectionFlat4WayRoadMesh(nullptr) {}

	/** 일반 왕복 도로 메쉬  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PlainRoadMesh;

	/** 4거리 평면 교차로 메쉬 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* IntersectionFlat4WayRoadMesh;
};

/** 길이에 따라 적용할 메시 LOD */
USTRUCT(BlueprintType)
struct FRoadLengthMesh
{
	GENERATED_BODY()
	FRoadLengthMesh() : RoadLength(0.0f), RoadMesh(nullptr) {}

	/** RoadMesh가 적용될 도로길이의 하한값 입니다. 도로 길이가 이 값보다 클 경우 적용합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float RoadLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* RoadMesh;
};

/**
 * 교차로의 치수 정보를 담는 구조체
 * 도로 크기의 기준이 되는 x,y 상대 좌표, 도로 크기, 메시 유효 여부를 저장함. 
 */
struct FIntersectionDimensionInfo
{
	FIntersectionDimensionInfo()
	{
		X_BasePoint  = FVector::ZeroVector;
		Y_BasePoint  = FVector::ZeroVector;
		RoadWidth    = 0.0;
		bIsMeshValid = false;
	}

	FVector X_BasePoint;
	FVector Y_BasePoint;
	double  RoadWidth;
	bool    bIsMeshValid;
};

/** CityBlock 아이콘 저장용. */
USTRUCT(BlueprintType)
struct FCityBlockIcon
{
	GENERATED_BODY()

public:
	FCityBlockIcon()
		: CityBlockType(ECityBlockType::None), IconTexture(nullptr) {}

	FCityBlockIcon(const ECityBlockType InCityBlockType, UTexture2D* InIconTexture)
		: CityBlockType(InCityBlockType), IconTexture(InIconTexture) {}

	/** 일반 왕복 도로 메쉬  */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	ECityBlockType CityBlockType;

	/** 4거리 평면 교차로 메쉬 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconTexture;
};

/**
 * 버스가 지나가는 노선을 실질적으로 구축하는 Lane 정보를 담고 있습니다.
 * 1. 도로의 약포인터와 2. 차선 인덱스를 포함하는 "차선 경로"입니다.
 *
 * - 노선의 모양(spline), 차선 위의 버스 정류장, 차선을 통해 갈 수 있는 다음 도로 등
 * - 길찾기, 노선 구축에 필요한 최소한의 정보로 구성되어있습니다.
 */
USTRUCT(BlueprintType)
struct FLaneInfo
{
	GENERATED_BODY()

	FLaneInfo();
	FLaneInfo(AOmniRoad* InRoad, const int32 InLaneIdx);

	bool operator==(const FLaneInfo& InLaneInfo) const;

	USplineComponent* GetLaneSpline() const;
	float GetLength() const;
	
	FVector GetPointLocation(const int32 InputKey) const;
	FVector GetStartPoint() const;
	FVector GetEndPoint() const;
	
	FVector2D GetPointLocation2D(const int32 InputKey) const;
	FVector2D GetStartPoint2D() const;
	FVector2D GetEndPoint2D() const;

	static FLaneInfo UTurnInfo();
	
	/**
	 * 현재 차선을 통해 갈 수 있는 다음 도로의 차선들
	 * @return 
	 */
	TArray<FLaneInfo> GetNextLaneInfos() const;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniRoad> OmniRoad;
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 LaneIdx;

	/** LaneIdx에서 유턴 정보 표기용 */
	constexpr static int32 UTURN_LANE = -255;
};

template <>
struct std::hash<FLaneInfo>
{
	size_t operator()(const FLaneInfo& InLaneInfo) const;
};

/**
 * 이동, 환승 정보
 * 버스 노선, 승하차, 환승 정류장 정보 등
 */
USTRUCT(BlueprintType)
struct FTransferStep
{
	GENERATED_BODY()

	FTransferStep();
	FTransferStep(TWeakObjectPtr<AOmniStationBusStop> InBusStop, TWeakObjectPtr<AOmniLineBusRoute> InBusLine, const int32 InStartStopIdxInBusLine, const int32 InEndStopIdxInBusLine, const float InExpectedDist);

	bool operator==(const FTransferStep& InTransStep) const;

	static FTransferStep MakeLastStep(TWeakObjectPtr<AOmniStationBusStop> InBusStop);
	bool IsLastStep() const;

	/** 보유한 승차 정류장을 반환합니다. */
	AOmniStationBusStop* GetStartBusStop() const;
	/** 하차 정류장을 노선과 idx를 바탕으로 찾아서 반환합니다. */
	AOmniStationBusStop* GetEndBusStop() const;

	/** 승차 정류장 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniStationBusStop> BusStop;

	/** 탑승해야하는 버스 노선*/
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniLineBusRoute> BusLine;

	/**
	 * 노선에서 승차 정류장의 인덱스.
	 * 버스 노선의 BusStopDistanceList에서의 Index입니다.
	 * 노선 위에서 승차 정류장의 위치(dist)에 접근할 수 있게하는 장치입니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 StartBusStopDistanceIdx;

	/**
	 * 노선에서 하차 정류장의 인덱스.
	 * 버스 노선의 BusStopDistanceList에서의 Index입니다.
	 * 노선 위에서 승차 정류장의 위치(dist)에 접근할 수 있게하는 장치입니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 EndBusStopDistanceIdx;
	
	/** 해당 버스타고 이동하는 거리 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float ExpectedDist;
};

template <>
struct std::hash<FTransferStep>
{
	size_t operator()(const FTransferStep& InTransferStep) const;
};


/**
 * 전체 환승 경로를 나타내는 구조체입니다.
 * TArray<FTransferStep>의 래퍼입니다.
 * TArray<TArray<FTransferStep>> 구현용.
 */
USTRUCT(BlueprintType)
struct FTransferPath
{
	GENERATED_BODY()
	FTransferPath();
	explicit FTransferPath(const TArray<FTransferStep>& InTransferStepList);
	FTransferPath(const FTransferPath& Other);
	FTransferPath(FTransferPath&& Other) noexcept;

	FTransferPath& operator=(const FTransferPath& Other)
	{
		if (this == &Other)
			return *this;
		TransferStepList = Other.TransferStepList;
		return *this;
	}

	FTransferPath& operator=(FTransferPath&& Other) noexcept
	{
		if (this == &Other)
			return *this;
		TransferStepList = MoveTemp(Other.TransferStepList);
		return *this;
	}

	bool operator==(const FTransferPath& InOther) const
	{
		return (TransferStepList == InOther.TransferStepList);
	}

	/**
	 * 경로가 유효한지 확인합니다.
	 *  
	 * @return 다음의 경우 false입니다.
	 * 1. 배열이 비어있거나(경로 없음)
	 * 2. 배열의 길이가 1이고, 유일한 요소가 IsLastStep일 때(시작, 도착지가 이웃하거나 같은 경우)
	 */
	bool IsValidPath() const;

	float GetTotalPathDistance() const;

	void Reset(const uint32 NewSize = 0);

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FTransferStep> TransferStepList;

};

/**
 * 노선에서 정류장의 위치를 노선 Spline의 Distance로 표기하기 위한 구조체 입니다.
 * 버스 노선의 TArray 요소로 사용됩니다.
 * 노선->FBusStopDistanceList[n] 은 n번째 정류장과 함께 해당 정류장의 Dist를 나타냅니다.
 * 환승 경로 길찾기와 환승 경로 시각화에 사용됩니다.
 */
USTRUCT(BlueprintType)
struct FBusStopDistance
{
	GENERATED_BODY()

	FBusStopDistance();
	FBusStopDistance(const TWeakObjectPtr<AOmniStationBusStop>& InBusStop, float InDistance);

	/** Distance가 DISTANCE_NONE이거나 BusStop가 유효하지 않은 경우 false입니다. */
	bool IsValid() const;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniStationBusStop> BusStop;

	/**
	 * 노선에서 버스 정류장의 위치를 Distance로 나타냅니다.
	 * 노선 스플라인의 길이를 통해, 정류장 간의 거리와 상대 위치를 나타냅니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float Distance;

	constexpr static float DISTANCE_NONE = -1.0f;
};

/**
 * Waypoint용 버스 정류장과, LaneInfo 리스트에서 Waypoint의 Idx를 저장합니다.
 */
USTRUCT(BlueprintType)
struct FWayPoint
{
	GENERATED_BODY()
	
	FWayPoint();
	FWayPoint(const TWeakObjectPtr<AOmniStationBusStop>& BusStop, const int32 LaneInfoRouteIdx);

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniStationBusStop> BusStop;

	/** 노선을 구성하는 LaneInfo 리스트에서 이 waypoint 정류장이 있는 인덱스 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 LaneInfoRouteIdx;
};


/**
 * 환승 규칙.
 * 환승 길찾기에서 Passenger의 환승 성향에 대한 규칙을 담는 구조체입니다.
 * 환승 저항만 있습니다.
 */
USTRUCT(BlueprintType)
struct FTransferRule
{
	GENERATED_BODY()
	explicit FTransferRule(const float Resistance = 0.0f) : Resistance(Resistance) {}

	float GetResistance() const { return Resistance; }

	/** 환승저항. 환승할 때마다 거리 계산에 더해지는 값 입니다. */
	UPROPERTY(EditAnywhere)
	float Resistance;
};


/**
 * CityBlock 하부 구역.
 * Passenger의 출발, 도착하는 지점인 SubSector에 필요한 데이터입니다.
 */
USTRUCT()
struct FSubSector
{
	GENERATED_BODY()
	FSubSector();
	explicit FSubSector(const TWeakObjectPtr<AOmniCityBlock>& InCityBlock
	                  , const TWeakObjectPtr<AOmniStationBusStop>& InBusStop
	                  , const FVector& InRelativeLocation);

	FVector GetWorldLocation() const;
	FVector GetRelativeLocation() const;

	void AddVisitor(AOmniPassenger* InPassenger);
	void RemoveVisitor(AOmniPassenger* InPassenger);

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniCityBlock> CityBlock;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniStationBusStop> BusStop;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector RelativeLocation;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TSet<TWeakObjectPtr<AOmniPassenger>> CurrentVisitorList;
};

/**
 * CityBlock과 SubSector의 Idx로 특정 SubSector의 정보에 접근합니다.
 * 
 */
USTRUCT()
struct FSectorInfo
{
	GENERATED_BODY()
	FSectorInfo();
	FSectorInfo(const TWeakObjectPtr<AOmniCityBlock>& InCityBlock, const int32 InSectorIdx);

	bool IsValid() const;

	FSubSector* GetBlockSubSector() const;

	AOmniStationBusStop* GetSectorBusStop() const;

	TWeakObjectPtr<AOmniCityBlock> CityBlock;
	int32 SectorIdx;
};

/** 여정 방향 */
enum class EJourneyDirection
{
	HomeToDest,
	DestToHome,
};

/** 다음 구조의 익명 union을 대체함.
 	union
	{
		FSectorInfo SubSectorInfo;
		TWeakObjectPtr<AOmniStationBusStop> BusStop;
		TWeakObjectPtr<AOmniVehicleBus> Bus;
	};
 */
using FLocationVariant = std::variant<FSectorInfo, TWeakObjectPtr<AOmniStationBusStop>, TWeakObjectPtr<AOmniVehicleBus>>;

/** 승객의 위치 정보 */
USTRUCT()
struct FPassengerLocationInfo
{
	GENERATED_BODY()
	FPassengerLocationInfo()
		: LocationInfo(), CurrentState(EState::None) {}

	/** 현재위치 */
	enum class EState : uint8
	{
		None, Disable, SubSector, BusStop, Bus, Sidewalk,
	};

	/** FLocationVariant의 index로 EState를 찾습니다. */
	static EState IndexToEState(const int32 InIdx);

	void EntrySubSector(const FSectorInfo& InSectorInfo);
	void EntryBusStop(AOmniStationBusStop* InBusStop);
	void EntryBus(AOmniVehicleBus* InBus);
	void EntryLocation(const FLocationVariant& InLocationVariant);

	EState               GetCurrentState() const { return CurrentState; }
	FSectorInfo          GetSubSector() const;
	AOmniStationBusStop* GetBusStop() const;
	AOmniVehicleBus*     GetBus() const;

private:
	/** 현재 위치한 곳 */
	FLocationVariant LocationInfo;
	EState CurrentState;
};

/** 현재 여정 상태 */
enum class EJourneyState
{
	None,             //
	FindingPath,      // 길찾는 중
	NoPath,           // 경로 없음
	Traveling,        // 여행 중
	ArrivalCompleted, // 도착완료
	Size,
};
