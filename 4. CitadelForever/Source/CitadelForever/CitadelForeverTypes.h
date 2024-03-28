// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "CitadelForeverTypes.generated.h"

/** uint8 enum class를 uint8로 변환*/
#define EnumToInt(EN) static_cast<uint8>(EN)

/** 공용 상수*/
class CitadelConstant
{
public:
	/** 단위 길이 당 월드 길이. 언리얼 1단위는 1cm와 동일. 프로젝트 단위 길이는 1m */
	static constexpr double Unit_Range = 100.;
};

/** 스텐실 외곽선 색상*/
UENUM()
enum class EStencilOutline : uint8
{
	None = 0,
	Preview_Buildable,
	Preview_Unbuildable,
	Friendly_Hover,
	Friendly_Select,
	Enemy_Hover,
	Enemy_Select,
};

UENUM()
enum class EBuildingType : uint8
{
	None,
	Default,
	Default_Rampart,
	Default_Tower,
	Cube_Wall,
	Size,
};

/** 게임 태그 FName 모음*/
struct ECitadelTag
{
	static const FName CitadelBuilding;
	static const FName CitadelTower;
	static const FName CitadelRampart;
	
};

/** 저장 등 사용되는 건물 기초 정보. 타입, 위치, 방향 레벨*/
USTRUCT()
struct FBuildingInfo
{
	GENERATED_BODY()
	FBuildingInfo
	(
		EBuildingType InBT = EBuildingType::None,
		FVector InLoc = FVector::ZeroVector,
		FRotator InRot = FRotator::ZeroRotator,
		uint32 InBuildingLv = 1
	)
	: BuildingType(InBT)
	, Location(InLoc)
	, Rotator(InRot)
	, BuildingLevel(InBuildingLv)
	{
	}

	EBuildingType BuildingType;
	FVector Location;
	FRotator Rotator;

	uint32 BuildingLevel;
};

/** 빌딩 클래스 및 UI 이미지 저장*/
USTRUCT()
struct FBuildingTemplate
{
	GENERATED_BODY()
	FBuildingTemplate() : BuildingType(EBuildingType::None), CitadelBuilding(nullptr), CitadelBuildingSlotImage(nullptr){}

	UPROPERTY(VisibleAnywhere)
	EBuildingType BuildingType;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ACitadelBuilding> CitadelBuilding;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UTexture2D* CitadelBuildingSlotImage;
	
};

USTRUCT(BlueprintType)
struct FCitadelTowerStatusData : public FTableRowBase
{
	GENERATED_BODY()
	FCitadelTowerStatusData()
	: BuildingType(EBuildingType::None)
	, BuildingName(FString())
	, BasicMaxHP()
	, AttackPower()
	, AttackRange()
	, AttackSpeed()
	, Defense()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	EBuildingType BuildingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	FString BuildingName;

	/** 기초 체력, 1레벨 체력*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 BasicMaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 AttackPower;

	/** 공격 범위*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	float AttackRange;

	/** 공격 속도. 높을 수록 빠르게 공격함. 공격 딜레이는 공속의 역수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 AttackSpeed;

	/** 방어력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 Defense;
};