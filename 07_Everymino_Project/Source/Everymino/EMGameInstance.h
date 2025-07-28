// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EMGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class EVERYMINO_API UEMGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
	virtual void BeginDestroy() override;
	class AEMPlayData* GetEMPlayData();
private:	
	UPROPERTY()
	class AEMPlayData* EMPlayData;
};
