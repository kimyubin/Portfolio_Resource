// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMController.h"
#include "BrickBoardManager.h"

AEMController::AEMController()
{
}

void AEMController::BeginPlay()
{
	Super::BeginPlay();
	//마우스 입력 모드: 게임 + UI 모두 입력 가능.
	//클릭시 커서 사라지는 거 방지. 커서 보임. 마우스 오버, 클릭 이벤트 사용.
	FInputModeGameAndUI InputModeGameUI = FInputModeGameAndUI();
	InputModeGameUI.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeGameUI);
	SetShowMouseCursor(true);
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
	DefaultMouseCursor = EMouseCursor::Default;
	
	SetupBoardManagerInput();
}

void AEMController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AEMController::SetupInputComponent()
{
	Super::SetupInputComponent();	
}
void AEMController::SetupBoardManagerInput()
{
	if (InputComponent == nullptr) return;

	auto BindPressKey = [&](const FKey InKey, FInputActionHandlerSignature::TMethodPtr<AEMController> Func)
	{
		InputComponent->BindKey<AEMController>(InKey, IE_Pressed, this, Func);
	};
	auto BindReleaseKey = [&](const FKey InKey, FInputActionHandlerSignature::TMethodPtr<AEMController> Func)
	{
		InputComponent->BindKey<AEMController>(InKey, IE_Released, this, Func);
	};
	//게임보드 사분면 선택
	BindPressKey(EBindKey::SelectNorth, &AEMController::PressSelectNorthKey);
	BindPressKey(EBindKey::SelectEast, &AEMController::PressSelectEastKey);
	BindPressKey(EBindKey::SelectSouth, &AEMController::PressSelectSouthKey);
	BindPressKey(EBindKey::SelectWest, &AEMController::PressSelectWestKey);

	//브릭 조작
	BindPressKey(EBindKey::MoveUp, &AEMController::PressMoveUpKey);
	BindPressKey(EBindKey::MoveRight, &AEMController::PressMoveRightKey);
	BindPressKey(EBindKey::MoveDown, &AEMController::PressMoveDownKey);
	BindPressKey(EBindKey::MoveLeft, &AEMController::PressMoveLeftKey);

	BindPressKey(EBindKey::HardDrop, &AEMController::PressHardDropKey);
	
	BindPressKey(EBindKey::Clockwise, &AEMController::PressClockwiseKey);
	BindPressKey(EBindKey::AntiClockwise, &AEMController::PressAntiClockwiseKey);

	BindReleaseKey(EBindKey::MoveUp, &AEMController::ReleaseMoveUpKey);
	BindReleaseKey(EBindKey::MoveRight, &AEMController::ReleaseMoveRightKey);
	BindReleaseKey(EBindKey::MoveDown, &AEMController::ReleaseMoveDownKey);
	BindReleaseKey(EBindKey::MoveLeft, &AEMController::ReleaseMoveLeftKey);

    //https://gist.github.com/samuelmaddock/c0582aa8bf53c1ccd86a38463de261d3
	// InputKeysDelegate.BindUFunction(this,FName("PressKey"));
	//
	// // Listen for key pressed
	// FInputKeyBinding KBP(FInputChord(EKeys::AnyKey, false, false, false, false), EInputEvent::IE_Pressed);
	// KBP.bConsumeInput = true;
	// KBP.bExecuteWhenPaused = false;
	// KBP.KeyDelegate.GetDelegateWithKeyForManualSet().BindLambda([=](const FKey& Key)
	// {
	// 	InputKeysDelegate.ExecuteIfBound(Key, true);
	// });
	// InputComponent->KeyBindings.Add(KBP);
	//
	// // Listen for key released
	// FInputKeyBinding KBR(FInputChord(EKeys::AnyKey, false, false, false, false), EInputEvent::IE_Released);
	// KBR.bConsumeInput = true;
	// KBR.bExecuteWhenPaused = false;
	// KBR.KeyDelegate.GetDelegateWithKeyForManualSet().BindLambda([=](const FKey& Key)
	// {
	// 	InputKeysDelegate.ExecuteIfBound(Key, false);
	// });
	// InputComponent->KeyBindings.Add(KBR);
	
}
void AEMController::SetMyBoardManager(ABrickBoardManager* InBrickBoardManager)
{
	MyBoardManager = InBrickBoardManager;
}
void AEMController::PressKey(const FKey& InKey, bool bKeyPressed)
{
}

void AEMController::PressSelectNorthKey() { MyBoardManager->SetCurrentControlZone(EDropStartDirection::North); }
void AEMController::PressSelectEastKey() { MyBoardManager->SetCurrentControlZone(EDropStartDirection::East); }
void AEMController::PressSelectSouthKey() { MyBoardManager->SetCurrentControlZone(EDropStartDirection::South); }
void AEMController::PressSelectWestKey() { MyBoardManager->SetCurrentControlZone(EDropStartDirection::West); }

void AEMController::PressMoveUpKey() { MyBoardManager->SetPressInput(EMInput::UpMove); }
void AEMController::PressMoveRightKey() { MyBoardManager->SetPressInput(EMInput::RightMove); }
void AEMController::PressMoveDownKey() { MyBoardManager->SetPressInput(EMInput::DownMove); }
void AEMController::PressMoveLeftKey() { MyBoardManager->SetPressInput(EMInput::LeftMove); }

void AEMController::PressHardDropKey() { MyBoardManager->PressedHardDrop(); }

void AEMController::PressClockwiseKey() { MyBoardManager->PressedRotateInput(EMInput::Clockwise); }
void AEMController::PressAntiClockwiseKey() { MyBoardManager->PressedRotateInput(EMInput::AntiClockwise); }

void AEMController::ReleaseMoveUpKey() { MyBoardManager->SetReleasedInput(EMInput::UpMove); }
void AEMController::ReleaseMoveRightKey() { MyBoardManager->SetReleasedInput(EMInput::RightMove); }
void AEMController::ReleaseMoveDownKey() { MyBoardManager->SetReleasedInput(EMInput::DownMove); }
void AEMController::ReleaseMoveLeftKey() { MyBoardManager->SetReleasedInput(EMInput::LeftMove); }
