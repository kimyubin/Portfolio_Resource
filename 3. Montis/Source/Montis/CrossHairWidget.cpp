// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CrossHairWidget.h"

#include "Montis.h"
#include "MontisGameInstance.h"
#include "MontisGameUIsHandler.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

UCrossHairWidget::UCrossHairWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DefaultTargetPosition = 24;
	CurrentPosition = DefaultTargetPosition;
	AimTargetPosition = 11;
	JumpTargetPosition = 40;
	RunTargetPosition = 37;

	LineMovementSpeed = 65;
	IsAimAnimRun = false;
}

void UCrossHairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto UIHandler = GetGameInstance<UMontisGameInstance>()->GetMontisGameUIsHandler();
	if (IsValid(UIHandler))
	{
		UIHandler->SetCrossHairWidget(this);
	}
}

void UCrossHairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (IsAimAnimRun)
	{
		if (CurrentPosition > AimTargetPosition)
		{
			SetLineDistanceFromCenter(CurrentPosition - (InDeltaTime * FMath::Clamp(LineMovementSpeed, 0.1, 1000.)));
		}
	}
	else
	{
		if (CurrentPosition < DefaultTargetPosition)
		{
			SetLineDistanceFromCenter(CurrentPosition + (InDeltaTime * FMath::Clamp(LineMovementSpeed, 0.1, 1000.)));
		}
	}
}


void UCrossHairWidget::PlayAimingAnim(bool IsAim)
{
	IsAimAnimRun = IsAim;
}

void UCrossHairWidget::PlayJumpAnim(bool IsJump)
{
}

void UCrossHairWidget::PlayRunAnim(bool IsRun)
{
}

void UCrossHairWidget::SetLineDistanceFromCenter(double Distance)
{
	CurrentPosition = Distance;

	UCanvasPanelSlot* Line_UpSlot = Cast<UCanvasPanelSlot>(Line_Up->Slot);
	UCanvasPanelSlot* Line_DownSlot = Cast<UCanvasPanelSlot>(Line_Down->Slot);
	UCanvasPanelSlot* Line_LeftSlot = Cast<UCanvasPanelSlot>(Line_Left->Slot);
	UCanvasPanelSlot* Line_RightSlot = Cast<UCanvasPanelSlot>(Line_Right->Slot);

	FVector2D PrvUpPos = Line_UpSlot->GetPosition();
	FVector2D PrvDownPos = Line_DownSlot->GetPosition();
	FVector2D PrvLeftPos = Line_LeftSlot->GetPosition();
	FVector2D PrvRightPos = Line_RightSlot->GetPosition();

	PrvUpPos.Y = -Distance;
	PrvDownPos.Y = Distance;
	PrvLeftPos.X = -Distance;
	PrvRightPos.X = Distance;

	Line_UpSlot->SetPosition(PrvUpPos);
	Line_DownSlot->SetPosition(PrvDownPos);
	Line_LeftSlot->SetPosition(PrvLeftPos);
	Line_RightSlot->SetPosition(PrvRightPos);
}

void UCrossHairWidget::SetCameraDistanceAndFOV(const float& InArmLength, const float& InFOV)
{
	Test_Distance_Value->SetText(FText::AsNumber(InArmLength));
	Test_FOV_Value->SetText(FText::AsNumber(InFOV));
}
