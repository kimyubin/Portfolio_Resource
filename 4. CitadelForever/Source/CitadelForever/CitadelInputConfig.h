// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "CitadelInputConfig.generated.h"

/**
 * 입력 구성 정보들을 받아서 담는 데이터 에셋
 * 각 InputAction을 BP에서 받음
 * 받은 InputAction 구성 정보를 키액션에 바인딩함.
 */
UCLASS()
class CITADELFOREVER_API UCitadelInputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Citadel_IA_Move;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Citadel_IA_WheelAxis;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Citadel_IA_LeftButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* Citadel_IA_RightButton;
};
