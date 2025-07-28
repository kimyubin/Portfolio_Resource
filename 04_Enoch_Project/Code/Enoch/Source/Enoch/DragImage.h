// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Common/EnochFreeLancerData.h"
#include "Components/Image.h"

#include "DragImage.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UDragImage : public UUserWidget
{
	GENERATED_BODY()
	public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* DragFreelancerImg;

	void SetDragImg(uint8 FLTID);
};
