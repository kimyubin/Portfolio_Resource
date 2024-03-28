// Fill out your copyright notice in the Description page of Project Settings.


#include "MontisGameUIsHandler.h"

#include "CrossHairWidget.h"

void UMontisGameUIsHandler::SetCrossHairWidget(UCrossHairWidget* InCH)
{
	CrossHair = InCH;	
}
UCrossHairWidget* UMontisGameUIsHandler::GetCrossHairWidget()
{
	return CrossHair.Get();
}
