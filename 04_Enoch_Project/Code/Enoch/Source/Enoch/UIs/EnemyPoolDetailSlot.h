// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enoch/Common/EnochFreeLancerData.h"
#include "EnemyPoolDetailSlot.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class ENOCH_API UEnemyPoolDetailSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UImage* DetailSlotImg;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DetailSlotNum;
	//용병이미지를 머티리얼 포인터로 받아서 이미지 갱신
	void SetDetailSlot(UMaterialInterface* Material, const int& FreeLancerCount );
	
	
};
