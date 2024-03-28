// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MontisGameUIsHandler.generated.h"

/**
 * 
 */
UCLASS()
class MONTIS_API UMontisGameUIsHandler : public UObject
{
	GENERATED_BODY()
public:
	
	void SetCrossHairWidget(class UCrossHairWidget* InCH);
	class UCrossHairWidget* GetCrossHairWidget();
	
	
	TWeakObjectPtr<class UCrossHairWidget> CrossHair;
};
