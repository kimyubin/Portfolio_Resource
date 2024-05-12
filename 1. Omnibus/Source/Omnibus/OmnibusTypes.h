// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.generated.h"

/** uint8 enum class를 uint8로 변환*/
#define EnumToInt(EN) static_cast<uint8>(EN)

/** 공용 상수*/
struct FOmniConst
{
	/** 프로젝트 단위 길이. 1칸을 의미. 단위 길이 당 월드의 길이. 언리얼 1단위는 1cm와 동일. */
	static constexpr double Unit_Length = 10.0;
};

UENUM()
enum class EOmniPlayMode : uint8
{
	None,
	Move,
	Edit,
	Build,
};

UENUM()
enum class EBusType : uint8
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
	static const FName RoadConnector;

	/**
	 * 버스 정류장과 도로 등, 시스템적으로 서로 감지해야하는 콜리전 간 사용.
	 * SysDetector 오버랩 전용. 그외 Ignore. 
	 * 게임 플레이 중 미사용. (정류장에 접근한 버스를 감지하는 기능 등에는 사용 안함.)
	 */
	static const FName SysDetector;

	/**
	 * 버스와 버스, 버스와 정류장 등 감지하는 콜리전
	 * 
	 */
	static const FName BusDetector;
	
};

struct EOmniColor
{
	/**
	 * 사전에 정의된 컬러 배열에서 순서대로 색을 반환. 서로 다른 색을 쓰고 싶을 경우 사용.
	 * @param CountFactor 배열 순번이 늘어나는 배수. 색을 건너뛸 수 있음.
	 * @return 순서에 맞는 색.
	 */
	static FColor GetNextColor(const int CountFactor = 1);

	/**
	 * HSV 색조(Hue)를 N번 분할한 후 순서대로 가져옵니다. 배수를 조절해 색을 건너뛸 수 있습니다.
	 *
	 * @param ColorSplitNumber 분할 횟수. 최대 255까지 나눌 수 있음. 색상 간격의 N/255 만큼 차이나게됨.
	 * @param CountFactor 순번이 늘어나는 배수. 색을 건너뛸 수 있음.
	 * @return N/255 다음 순번의 색
	 */
	static FColor GetNextColorByHue(const uint8 ColorSplitNumber = 32, const uint8 CountFactor = 1);

	/** 현재 무지개색 */
	static std::vector<FColor> ColorSet;
};

/** 도로용 스태틱 메시 보관용 세트 */
USTRUCT(BlueprintType)
struct FRoadStaticMeshSet
{
	GENERATED_BODY()

public:
	FRoadStaticMeshSet(): PlainRoadMesh(nullptr), IntersectionFlat4WayRoadMesh(nullptr)
	{
	}

	/** 일반 왕복 도로 메쉬  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PlainRoadMesh;

	/** 4거리 평면 교차로 메쉬 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* IntersectionFlat4WayRoadMesh;
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
	FCityBlockIcon(): CityBlockType(ECityBlockType::None), IconTexture(nullptr)
	{
	}

	FCityBlockIcon(ECityBlockType InCityBlockType, UTexture2D* InIconTexture)
		: CityBlockType(InCityBlockType), IconTexture(InIconTexture)
	{
	}

	/** 일반 왕복 도로 메쉬  */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	ECityBlockType CityBlockType;

	/** 4거리 평면 교차로 메쉬 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UTexture2D* IconTexture;
};
