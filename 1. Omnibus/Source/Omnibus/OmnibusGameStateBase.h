// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OmnibusGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class OMNIBUS_API AOmnibusGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void HandleBeginPlay() override;

	/**
	 * 모든 액터의 BeginPlay() 호출 이후 호출됨.
	 * HandleBeginPlay()에서 모든 액터의 BeginPlay() 호출 후, HandlePostBeginPlay() 호출.  
	 */
	virtual void HandlePostBeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
