// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFieldDropProtector.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UEnochFieldDropProtector::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	Draghover=true;
}
void UEnochFieldDropProtector::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	Draghover=false;	
}

void UEnochFieldDropProtector::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	Draghover=true;
}
void UEnochFieldDropProtector::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	Draghover=false;	

}
