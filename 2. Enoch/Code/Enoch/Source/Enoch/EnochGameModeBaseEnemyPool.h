// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnochGameModeBase.h"
#include "EnochGameModeBaseEnemyPool.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API AEnochGameModeBaseEnemyPool : public AEnochGameModeBase
{
	GENERATED_BODY()
protected:
	virtual void SubBeginPlay() override;
};
