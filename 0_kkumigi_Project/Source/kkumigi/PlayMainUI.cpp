// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayMainUI.h"

#include "InventoryUI.h"

void UPlayMainUI::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayMainUI::ToggleInventoryWidget()
{
	BP_InventoryUI->ToggleVisibility();
}

bool UPlayMainUI::IsInventoryVisible() const
{
	return BP_InventoryUI->IsInventoryVisible();
}
