// Fill out your copyright notice in the Description page of Project Settings.


#include "KuUserWidget.h"

#include "FieldItemFactory.h"
#include "ItemManager.h"
#include "KuGameInstance.h"

void UKuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UKuUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	GetItemManager()->GetFieldItemFactory()->SetHoverUI(true);
}

void UKuUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	GetItemManager()->GetFieldItemFactory()->SetHoverUI(false);
}

FReply UKuUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

FReply UKuUserWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

UKuGameInstance* UKuUserWidget::GetKuGameInstance() const
{
	return GetGameInstance<UKuGameInstance>();
}

UKuPlayData* UKuUserWidget::GetKuPlayData() const
{
	return GetKuGameInstance()->GetKuPlayData();
}

UKuSaveManager* UKuUserWidget::GetKuSaveManager() const
{
	return GetKuGameInstance()->GetKuSaveManager();
}

UItemManager* UKuUserWidget::GetItemManager() const
{
	return GetKuGameInstance()->GetItemManager();
}

UKuUIManager* UKuUserWidget::GetKuUIManager() const
{
	return GetKuGameInstance()->GetUIManager();
}
