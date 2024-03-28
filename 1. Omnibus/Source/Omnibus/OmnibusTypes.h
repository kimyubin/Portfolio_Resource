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

/** 게임 태그용 FName 모음*/
struct EOmniComponentTag
{
	static const FName StartPoint_RoadSpline;
	static const FName EndPoint_RoadSpline;
	static const FName StartPoint_Lane;
	static const FName EndPoint_Lane;
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

