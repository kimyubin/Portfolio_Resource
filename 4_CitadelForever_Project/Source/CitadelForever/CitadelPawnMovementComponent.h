// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawnMovement.h"
#include "CitadelPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CITADELFOREVER_API UCitadelPawnMovementComponent : public USpectatorPawnMovement
{
	GENERATED_BODY()
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;  

	
};
