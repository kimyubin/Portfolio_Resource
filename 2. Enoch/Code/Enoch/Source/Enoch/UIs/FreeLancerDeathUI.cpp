// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FreeLancerDeathUI.h"


void UFreeLancerDeathUI::NativeConstruct()
{
	Super::NativeConstruct();

	isDeathTrigger=true;
}

void UFreeLancerDeathUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//사망 애니메이션. 사망시 1회 작동 및 다시 전투 시작 후 작동하는 용도
	if(OwnerFreeLancer.IsValid())
	{		
		if(OwnerFreeLancer->GetState()==FreeLancerState::Dead && isDeathTrigger)
		{
			PlayAnimation(OpacityAnimation);
			isDeathTrigger=false;
		}
		if(OwnerFreeLancer->GetState() != FreeLancerState::Dead)
		{
			isDeathTrigger=true;
		}
	}
}
