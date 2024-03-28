// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "UIs/UniformSlot.h"

#include "EnochDragDropOP.generated.h"
DECLARE_DELEGATE_OneParam(FDele_Drag_Spawn, FVector2D)
/**
 * 
 */
UCLASS()
class ENOCH_API UEnochDragDropOP : public UDragDropOperation
{
	GENERATED_BODY()
public:	
	virtual void DragCancelled_Implementation(const FPointerEvent& PointerEvent) override;
	/** 드래그 드롭 스폰용 델리게이트*/
	FDele_Drag_Spawn FDeleDragDropSpawnFL;
	
	UPROPERTY()
	UUserWidget* WidgetRef;
	
};
