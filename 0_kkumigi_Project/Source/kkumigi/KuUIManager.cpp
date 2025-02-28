// Fill out your copyright notice in the Description page of Project Settings.


#include "KuUIManager.h"

#include "KuUserWidget.h"
#include "PlayMainUI.h"
#include "Blueprint/UserWidget.h"

#include "UtlLog.h"

void UKuUIManager::Init()
{
	Super::Init();
}

void UKuUIManager::ChangeWidget(const TSubclassOf<UKuUserWidget> NewWidgetClass)
{
	if (CurrentWidget != nullptr)
	{
		CurrentWidget->RemoveFromParent();
		CurrentWidget = nullptr;
	}
	if (NewWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UKuUserWidget>(GetWorld(), NewWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void UKuUIManager::SwitchToPlayMainUI()
{
	ChangeWidget(PlayMainWidgetClass);
}

void UKuUIManager::ToggleInventory() const
{
	// 현재 창에서 인벤토리 열 수 있는 지 확인.
	if (UPlayMainUI* PlayMainUI = Cast<UPlayMainUI>(CurrentWidget))
	{
		PlayMainUI->ToggleInventoryWidget();
	}
}

bool UKuUIManager::IsInventoryOpen() const
{
	// 현재 창에서 인벤토리 열려있는지 확인
	if (UPlayMainUI* PlayMainUI = Cast<UPlayMainUI>(CurrentWidget))
	{
		return PlayMainUI->IsInventoryVisible();
	}
	return false;
}
