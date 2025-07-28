// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnochFieldDropProtector.generated.h"

/**
 * 드래그 드롭 시 UI창 뚫는 현상 방지용.
 * 그외에 UI를 뚫고 필드 용병 정보 읽는 현상도 방지함.
 */
UCLASS()
class ENOCH_API UEnochFieldDropProtector : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	bool IsDragHoverd(){return Draghover;}
private:
	bool Draghover;
	
};
