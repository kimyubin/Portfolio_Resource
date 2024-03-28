// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MontisPlayData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class MONTIS_API UMontisPlayData : public UObject
{
	GENERATED_BODY()
public:
	UMontisPlayData();

	FVector3d CurrentLocation;
};
