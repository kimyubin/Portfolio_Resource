// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelUserWidget.h"

#include "CitadelPlayerController.h"
#include "Blueprint/WidgetTree.h"

void UCitadelUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UCitadelUserWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	GetOwningPlayer<ACitadelPlayerController>()->HoverUI();	
}

void UCitadelUserWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	GetOwningPlayer<ACitadelPlayerController>()->UnHoverUI();
}
FReply UCitadelUserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}
FReply UCitadelUserWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}
