// Fill out your copyright notice in the Description page of Project Settings.


#include "KuPlayerController.h"

#include "FieldItem.h"
#include "KuGameInstance.h"
#include "KuInputConfig.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "FieldItemFactory.h"
#include "InputMappingContext.h"
#include "ItemManager.h"
#include "KuUIManager.h"

#include "Kismet/GameplayStatics.h"


void AKuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//마우스 입력 모드 게임 + UI 모두 입력 가능.
	//클릭시 커서 사라지는 거 방지
	FInputModeGameAndUI InputModeGameUI = FInputModeGameAndUI();
	InputModeGameUI.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeGameUI);
	SetShowMouseCursor(true);

	bEnableMouseOverEvents = true;
	bEnableClickEvents     = true;

}

UKuGameInstance* AKuPlayerController::GetKuGameInstance() const
{
	return GetGameInstance<UKuGameInstance>();
}

void AKuPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnTrackCursor.Unbind();
}

void AKuPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	OnTrackCursor.ExecuteIfBound(this);

	ExecuteTrackCursor();
}

void AKuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//입력 다시 맵핑
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		Subsystem->AddMappingContext(InputMapping, 0);
		// InputMapping->MapKey(InputActionsConfig->IA_Inventory, EKeys::J);
	}

	// 키 액션 바인드
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(InputActionsConfig->IA_LeftButton, ETriggerEvent::Triggered, this, &AKuPlayerController::OnLeftButton);
		EnhancedInputComponent->BindAction(InputActionsConfig->IA_RightButton, ETriggerEvent::Triggered, this, &AKuPlayerController::OnRightButton);
		EnhancedInputComponent->BindAction(InputActionsConfig->IA_Inventory, ETriggerEvent::Triggered, this, &AKuPlayerController::OnInventory);
		EnhancedInputComponent->BindAction(InputActionsConfig->IA_ItemRotate, ETriggerEvent::Triggered, this, &AKuPlayerController::OnItemRotate);

	}
}

void AKuPlayerController::OnLeftButton(const FInputActionValue& InputValue)
{
	if (AFieldItemFactory* ItemFactory = GetKuGameInstance()->GetItemManager()->GetFieldItemFactory())
	{
		ItemFactory->SetItemToReal();
	}
}

void AKuPlayerController::OnRightButton(const FInputActionValue& InputValue)
{
	if (AFieldItemFactory* ItemFactory = GetKuGameInstance()->GetItemManager()->GetFieldItemFactory())
	{
		ItemFactory->CancelPreviewItem();
	}
}

void AKuPlayerController::OnInventory(const FInputActionValue& InputValue)
{
	if (UKuUIManager* UIManager = GetKuGameInstance()->GetUIManager())
	{
		UIManager->ToggleInventory();
	}
}

void AKuPlayerController::OnItemRotate(const FInputActionValue& InputValue)
{
	if (AFieldItemFactory* ItemFactory = GetKuGameInstance()->GetItemManager()->GetFieldItemFactory())
	{
		ItemFactory->RotateItem(InputValue.Get<float>());
	}	
}

void AKuPlayerController::ExecuteTrackCursor()
{
}

void AKuPlayerController::GetMultiHitResultsUnderCursor(TArray<FHitResult>& InHitResults, ECollisionChannel TraceChannel)
{
	FVector2D ScreenPosition;
	FVector WorldLocation;
	FVector WorldDirection;

	Cast<ULocalPlayer>(Player)->ViewportClient->GetMousePosition(ScreenPosition);

	//뷰포트에서 찍은 곳의 월드 위치 구해서 EndPoint에 넣기
	UGameplayStatics::DeprojectScreenToWorld(this, ScreenPosition, WorldLocation, WorldDirection);

	const FVector StartPoint = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->K2_GetActorLocation();
	const FVector EndPoint   = (WorldDirection * 15000) + WorldLocation;

	//카메라에서 마우스 찍은 곳까지 라인 트레이스.
	GetWorld()->LineTraceMultiByChannel(InHitResults, StartPoint, EndPoint, TraceChannel);
}

AKuPlayerController::FOnTrackCursor& AKuPlayerController::GetOnTrackCursorDelegate()
{
	return OnTrackCursor;
}
