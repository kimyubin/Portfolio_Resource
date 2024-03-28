// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelDefaultMainUI.generated.h"

/**
 * 각 레벨의 기본 UI의 부모 클래스.
 * 공용으로 쓰는 변수, 함수, 이미지 등을 관리.
 */
UCLASS()
class ENOCH_API ULevelDefaultMainUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	UPROPERTY(BlueprintReadOnly)
	class APulicSlotImgArr* PublicSlotImgArrActor;
	/**TID 기반 용병 슬롯 이미지 리턴.*/
	UMaterialInterface* GetSlotImg(int FreeLancerTID);
};
