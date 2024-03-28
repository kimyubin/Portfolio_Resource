// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FreeLancerUnifiedUpperUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "../Common/EnochFreeLancerData.h"

void UFreeLancerUnifiedUpperUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!OwnerFreeLancer.IsValid())
		return;
	if(OwnerFreeLancer->GetMaxHP()>0)
		HPProgressBar->SetPercent(static_cast<float>(OwnerFreeLancer->GetHP())/static_cast<float>(OwnerFreeLancer->GetMaxHP()));
	if(OwnerFreeLancer->GetMaxMP()>0)
		MPProgressBar->SetPercent(static_cast<float>(OwnerFreeLancer->GetMP())/static_cast<float>(OwnerFreeLancer->GetMaxMP()));
	
}
void UFreeLancerUnifiedUpperUI::SetOwnerFreeLancer(AEnochFreeLancer* InFL)
{
	OwnerFreeLancer = InFL;
	auto TID = InFL->GetData()->GetTID();
	const auto &temp = FreeLancerTemplate::GetFreeLancerTemplate(TID);
	FString stringName(temp->name.c_str());
	FText text = FText::FromString(*stringName);
	textName->SetText(text);
}
