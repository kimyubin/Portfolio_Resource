// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelArchUI.h"

#include "CitadelForeverGameInstance.h"
#include "CitadelPlayData.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetSwitcher.h"

void UCitadelArchUI::NativeConstruct()
{
	ActiveWidgetIdx = 0;
	
}

UCitadelPlayData* UCitadelArchUI::GetCitadelPlayData()
{	
	if(!MyCitadelPlayData.IsValid())
	{
		MyCitadelPlayData =
			GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData();
	}
	return MyCitadelPlayData.Get();
}
