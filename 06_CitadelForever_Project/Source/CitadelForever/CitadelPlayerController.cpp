// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelPlayerController.h"
#include "CitadelBuilding.h"
#include "CitadelBuildingFactory.h"
#include "CitadelDefaultPawn.h"
#include "CitadelForever.h"
#include "CitadelForeverGameInstance.h"
#include "CitadelPlayData.h"
#include "CitadelInputConfig.h"
#include "CitadelRampart.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void ACitadelPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//마우스 입력 모드 게임 + UI 모두 입력 가능.
	//클릭시 커서 사라지는 거 방지
	FInputModeGameAndUI InputModeGameUI = FInputModeGameAndUI();
	InputModeGameUI.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeGameUI);
	SetShowMouseCursor(true);
	
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
	
	IsPreviewMode = false;
	bHoverUI = false;

}

void ACitadelPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if(bHoverUI)
		return;
	if(IsPreviewMode)
	{
		TArray<FHitResult> HitResults;
		GetMultiHitResultsUnderCursor(HitResults, ECC_GameTraceChannel1);
		
		for (FHitResult& HitResult : HitResults)
		{
			AActor* HitActor = HitResult.GetActor();
			if (PC_Weak_PreviewBuilding.IsValid() && IsValid(HitActor))
			{
				if (PC_Weak_PreviewBuilding == HitActor)
					continue;				
				PC_Weak_PreviewBuilding->TrackCursorForPreview(HitResult.Location, HitActor);
				break;
			}
		}
	}
}

void ACitadelPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	//입력 다시 맵핑
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	// 키 액션 바인드
	EnhancedInputComponent->BindAction(InputActionsConfig->Citadel_IA_LeftButton, ETriggerEvent::Triggered, this, &ACitadelPlayerController::SpawnBuilding);
	EnhancedInputComponent->BindAction(InputActionsConfig->Citadel_IA_RightButton, ETriggerEvent::Triggered, this, &ACitadelPlayerController::CancelBuild);

}

void ACitadelPlayerController::SpawnAndTrackPreviewBuilding(EBuildingType PreviewType)
{
	if(PreviewType==EBuildingType::None)
		return;

	//다른 버튼 누르면, 빌딩 해제
	if(IsPreviewMode)
		CancelBuild();				
	IsPreviewMode = true;
	
	FHitResult Result;
	GetHitResultUnderCursor(ECC_Visibility, false, Result);	

	const auto CBF = GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelBuildingFactory();
	PC_Weak_PreviewBuilding = CBF->SpawnPreviewBuilding(FBuildingInfo(PreviewType, Result.Location));
}

void ACitadelPlayerController::SpawnBuilding()
{	
	if(bHoverUI)
		return;
	if(!IsPreviewMode)
		return;
	if(PC_Weak_PreviewBuilding.IsValid())
	{
		if(GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelBuildingFactory()->BuildPreviewToReal())
		{
			PC_Weak_PreviewBuilding.Reset();
			IsPreviewMode = false;
		}		
	}
}

void ACitadelPlayerController::CancelBuild()
{
	IsPreviewMode = false;
	if(PC_Weak_PreviewBuilding.IsValid())
	{
		PC_Weak_PreviewBuilding->Destroy();
		PC_Weak_PreviewBuilding.Reset();
	}	
}

void ACitadelPlayerController::GetMultiHitResultsUnderCursor(TArray<FHitResult>& InHitResults, ECollisionChannel TraceChannel)
{
	FVector2D ScreenPosition;
	FVector WorldLocation;
	FVector WorldDirection;	

	Cast<ULocalPlayer>(Player)->ViewportClient->GetMousePosition(ScreenPosition);

	//뷰포트에서 찍은 곳의 월드 위치 구해서 EndPoint에 넣기
	UGameplayStatics::DeprojectScreenToWorld(this, ScreenPosition, WorldLocation, WorldDirection);
	
	const FVector StartPoint = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->K2_GetActorLocation();
	const FVector EndPoint = (WorldDirection * 15000) + WorldLocation;

	//카메라에서 마우스 찍은 곳까지 라인 트레이스.
	//채널은 BuildingLineTrace. 기본적으로 오버랩으로 처리되어있음.
	GetWorld()->LineTraceMultiByChannel(InHitResults, StartPoint, EndPoint, TraceChannel);
}

void ACitadelPlayerController::HoverUI()
{
	//중복 실행 방지
	if(bHoverUI == true)
		return;
	if(PC_Weak_PreviewBuilding.IsValid())
	{
		PC_Weak_PreviewBuilding->SetBuildingVisibility(false);
	}
	bHoverUI = true;
}

void ACitadelPlayerController::UnHoverUI()
{
	//중복 실행 방지
	if(bHoverUI == false)
		return;
	if(PC_Weak_PreviewBuilding.IsValid())
	{
		PC_Weak_PreviewBuilding->SetBuildingVisibility(true);
	}
	bHoverUI = false;
}