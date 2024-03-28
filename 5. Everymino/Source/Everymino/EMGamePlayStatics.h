// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EMGamePlayStatics.generated.h"

class AEMUnitBrick;
struct FVector2I;
enum class EMInput : uint8;
enum class EDropStartDirection : uint8;
enum class UnitSkin : uint8;

/**
 * 공용 유틸리티 함수를 가지고 있는 Static Class
 */
UCLASS()
class EVERYMINO_API UEMGamePlayStatics : public UObject
{
	GENERATED_BODY()
public:
	
	/**
	 * 유닛 스폰 처리. 스폰하고 부모 액터에 붙여줌.
	 * @return 스킨이 비어있으면 nullptr 리턴, 그렇지 않으면 스폰후 유닛 리턴.
	 */
	static AEMUnitBrick* SpawnUnit(int InX, int InY, UnitSkin SkinEnum, AActor* ParentActor);

	/**
	 * 유닛 스폰 처리. 스폰하고 부모 액터에 붙여줌.
	 * @return 스킨이 비어있으면 nullptr 리턴, 그렇지 않으면 스폰후 유닛 리턴.
	 */
	static AEMUnitBrick* SpawnUnit(FVector2I InLocation, UnitSkin SkinEnum, AActor* ParentActor);

	/** 유닛 파괴 처리. 파괴하고 nullptr처리까지함.*/
	static void DestroyUnit(AEMUnitBrick** InUnit);

	/** 각 사분면의 소프트 다운(=가운데) 방향을 리턴함.*/
	static EMInput GetInsideDirection(EDropStartDirection InStartDirection);
	
	/** 각 사분면의 소프트 다운의 역방향(=바깥) 방향을 리턴함.*/
	static EMInput GetOutsideDirection(EDropStartDirection InStartDirection);
	
	/**
	 * 덧셈 오버플러우 방지. 
	 * T 타입의 최대값보다 큰 값이 들어오면 최대값만 적용.
	 * InOutAugend(피가수)에 Addend(가수)를 대입 연산함
	 * @param InOutAugend 더해지는 대상. 값이 변함
	 * @param Addend 더하고 싶은 수
	 */
	template< typename T >
	static void MaxLimitAdder(T& InOutAugend, const T Addend)
	{
		T Sub = TNumericLimits<T>::Max() - Addend;
		if(InOutAugend < Sub)
			InOutAugend += Addend;
		else	
			InOutAugend = TNumericLimits<T>::Max();	
	}
};
