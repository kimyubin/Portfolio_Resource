// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniObject.h"
#include "UObject/NoExportTypes.h"
#include "OmnibusUIsHandler.generated.h"

/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmnibusUIsHandler : public UOmniObject
{
	GENERATED_BODY()

public:
	UOmnibusUIsHandler();
	virtual void Initialize(UOmnibusGameInstance* InOmniGameInstance) override;
};
