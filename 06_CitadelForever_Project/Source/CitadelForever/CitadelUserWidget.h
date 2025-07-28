// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CitadelUserWidget.generated.h"

/**
 * 기본 위젯. 마우스 버튼 뚫는 현상 제거
 * 버튼이나 그림 등이 올라가는 위젯의 기본이 됨.
 */
UCLASS()
class CITADELFOREVER_API UCitadelUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	/** 버튼 클릭시 MouseLeave되는 거 방지*/
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	/** 버튼 클릭시 MouseLeave되는 거 방지*/
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
