// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniCityBlockWidget.h"

#include "OmnibusGameInstance.h"
#include "OmnibusPlayData.h"
#include "OmnibusUtilities.h"
#include "OmniCityBlock.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UOmniCityBlockWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UOmniCityBlockWidget::SetOwnerCityBlock(AOmniCityBlock* InCityBlock)
{
	if (IsValid(InCityBlock) == false)
		return;
	
	OwnerCityBlock = InCityBlock;
	UpdateTypeState(OwnerCityBlock->GetCurrentBlockType());
}

void UOmniCityBlockWidget::UpdateTypeState(const ECityBlockType InNextType)
{
	UTexture2D* CityBlockIcon = GetGameInstance<UOmnibusGameInstance>()->GetOmnibusPlayData()->GetCityBlockIcon(InNextType);
	TypeImg->SetBrushFromTexture(CityBlockIcon);
	TypeDescription->SetText(FText::FromString(OmniStr::EnumToString(InNextType)));
	
}
