// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EMDefinedType.generated.h"


/** uint8 enum class를 uint8로 변환*/
#define EnumToInt(EN) static_cast<uint8>(EN)
//UnitSkin::Empty
#define US_E UnitSkin::Empty
//UnitSkin::Wall
#define US_W UnitSkin::Wall
//UnitSkin::Default
#define US_D UnitSkin::Default

using namespace std;

/**
 * 사용자 정의 데이터 자료형 보관
 * enum class, struct 등을 보관함.
 */
UCLASS()
class EVERYMINO_API UEMDefinedType : public UObject
{
	GENERATED_BODY()
};

/** Brick의 모양. I,Sq,T,S,Z,J,L None이 있음. 템플릿 배열 번호라서 순서는 변경할 수 없음.	*/
enum class BrickType:uint8
{
	I,
	Sq,
	T,
	S,
	Z,
	J,
	L,
	None,

	Size
};

/** Brick의 방향. 상>우>하>좌 순.			*/
enum class BrickDirection:uint8
{
	UP,
	RIGHT,
	DOWN,
	LEFT,

	Size
};

/** 유닛의 스킨. empty는 빈칸임.			*/
UENUM(BlueprintType)
enum class UnitSkin:uint8
{
	Empty,
	Default,
	Skin,

	Wall,
	InvisibleWall,
	//겹침 체크용 투명 벽. 벽과 게임판 외부 공간을 채워서 체크함 

	Size
};

/** 입력 Brick 이동, 회전 방향 */
enum class EMInput:uint8
{
	None,
	AntiClockwise,
	//Left
	Clockwise,
	//Right
	SoftDrop,
	HardDrop,

	UpMove,
	DownMove,
	LeftMove,
	RightMove,

	Size
};

/** 드롭 시작 위치 지정. 북쪽부터 시계방향*/
UENUM()
enum class EDropStartDirection:uint8
{
	North,
	//북, 상
	East,
	//동, 우
	South,
	//남, 하
	West,
	//서, 좌

	Size
};

/** Vector2d를 대신하는 int32 기반 좌표 자료형*/
USTRUCT(BlueprintType)
struct FVector2I
{
	GENERATED_BODY()

	FVector2I(const int32 InX = 0, const int32 InY = 0)
		: X(InX), Y(InY)
	{
	}

	FVector2I operator+(const FVector2I& V) const;
	FVector2I operator+=(const FVector2I& V);

	static const FVector2I ZeroVector;

	int32 X;
	int32 Y;
};

USTRUCT(BlueprintType)
struct FUnitSkinData
{
	GENERATED_USTRUCT_BODY()
	FUnitSkinData(UnitSkin BT = UnitSkin::Default): BrickUnitSkin(BT), StaticMesh(nullptr)
	{
	}

	UPROPERTY(VisibleAnywhere)
	UnitSkin BrickUnitSkin;
	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;
};

//런타임 키변경용 바인딩 키
struct EBindKey
{
	static FKey MoveUp;
	static FKey MoveRight;
	static FKey MoveDown;
	static FKey MoveLeft;

	static FKey HardDrop;

	static FKey Clockwise;
	static FKey AntiClockwise;

	static FKey SelectNorth;
	static FKey SelectEast;
	static FKey SelectSouth;
	static FKey SelectWest;

	/** 현재 모든 키 설정 리턴*/
	static TArray<FKey> GetKeys()
	{
		return {
			MoveUp,
			MoveRight,
			MoveDown,
			MoveLeft,

			HardDrop,

			Clockwise,
			AntiClockwise,

			SelectNorth,
			SelectEast,
			SelectSouth,
			SelectWest,
		};
	}

	/** 모든 설정 한번에 적용*/
	static void SetKeys(const TArray<FKey>& InKeys)
	{
		if (InKeys.Num() != EnumToInt(EControlName::Size))
			return;
		MoveUp = InKeys[EnumToInt(EControlName::MoveUp)];
		MoveRight = InKeys[EnumToInt(EControlName::MoveRight)];
		MoveDown = InKeys[EnumToInt(EControlName::MoveDown)];
		MoveLeft = InKeys[EnumToInt(EControlName::MoveLeft)];

		HardDrop = InKeys[EnumToInt(EControlName::HardDrop)];

		Clockwise = InKeys[EnumToInt(EControlName::Clockwise)];
		AntiClockwise = InKeys[EnumToInt(EControlName::AntiClockwise)];

		SelectNorth = InKeys[EnumToInt(EControlName::SelectNorth)];
		SelectEast = InKeys[EnumToInt(EControlName::SelectEast)];
		SelectSouth = InKeys[EnumToInt(EControlName::SelectSouth)];
		SelectWest = InKeys[EnumToInt(EControlName::SelectWest)];
	}

	enum class EControlName
	{
		MoveUp,
		MoveRight,
		MoveDown,
		MoveLeft,

		HardDrop,

		Clockwise,
		AntiClockwise,

		SelectNorth,
		SelectEast,
		SelectSouth,
		SelectWest,

		Size
	};
};
