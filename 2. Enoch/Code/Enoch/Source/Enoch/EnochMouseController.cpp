// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnochMouseController.h"
#include "EnochActorFactory.h"
#include "EnochDragDropOP.h"
#include "EnochFight.h"
#include "EnochFreeLancer.h"
#include "EnochFieldCell.h"
#include "EnochGameInstance.h"
#include "EnochGameModeBase.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/World.h"


#include "Kismet/GameplayStatics.h"

void AEnochMouseController::BeginPlay()
{
	Super::BeginPlay();
	//SetMouseCursorWidget(GetMouseCursor(), ); //마우스 커서 모양 바꿀때 이 함수를 쓰는것같음. 확실치않음

	//마우스 입력 모드 게임 + UI 모두 입력 가능.
	//클릭시 커서 사라지는 거 방지
	FInputModeGameAndUI InputModeGameUI = FInputModeGameAndUI();
	InputModeGameUI.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeGameUI);	
	SetShowMouseCursor(true);
	
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
	

	//InputComponent->BindAction(TEXT("LeftClick"), EInputEvent::IE_Pressed, this, &AEnochMouseController::OnClick);
	InputComponent->BindAction(TEXT("Num1"), EInputEvent::IE_Pressed, this, &AEnochMouseController::StartFight);
	//InputComponent->BindAction(TEXT("RightClick"), EInputEvent::IE_Pressed, this, &AEnochMouseController::OnRightClick);
	InputComponent->BindAction(TEXT("Num2"), EInputEvent::IE_Pressed, this, &AEnochMouseController::EndFight);
	fightSimulator = nullptr;
	FriendEnemyCount = make_pair(0, 0);

	PrevFLSerialNumber = 0;
	APrevFreeLancer = nullptr;
}
void AEnochMouseController::DragDropSpawnFL(const FVector2D& ScreenVec2D)
{
	//스크린 좌표를 셀좌표로 변환하는 공용함수
	FHitResult Result = ScreenToCellLocation(ScreenVec2D);
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	FSelectedFreelancer SelectedFreeInfo = EGameInstance->SelectedFreeLancerInfo;

	//드래그 끝났으니 이미지 보임.
	//Drag Cancel 영역에서 처리하면 간섭 일어남.
	//Result 연산이 끝나고 나서 켜줘야 용병에 붙은 드랍 감지용 캡슐 콜라이더와 간섭이 없음.
	if(SelectedFreeInfo.DraggedFreeLancer!=nullptr)
		SelectedFreeInfo.DraggedFreeLancer->SetImageVisualOnOff(true);
	
	//UI 위에 있을 때 작동안함.
	if (EGameInstance->MyPlayMenuUI->IsHoverUI())
		return;

	//바닥있을때만 스폰. 바닥에 내려놓을때만 스폰. 용병 위에서도 스폰가능
	if (Result.IsValidBlockingHit())
	{
		FreeLancerSpawn(Result);
	}		
}
FHitResult AEnochMouseController::ScreenToCellLocation(const FVector2D& ScreenVec2D)
{
	FHitResult Result;

	//DDop에 델리케이트 등록된 다음 받아온 화면 위치를 바탕으로, 라인 캐스트하기
	//EndPoint=레이캐스트 끝부분 구하기. 드래그 드롭 좌표 -> 뷰포트 스크린 좌표 -> 월드 방향과 위치 
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	FVector WorldLocation;
	FVector WorldDirection;
	auto StartPoint= UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->K2_GetActorLocation();
	FVector EndPoint;

	//뷰포트에서 찍은 곳의 월드 위치 구해서 EndPoint에 넣기
	USlateBlueprintLibrary::AbsoluteToViewport(this,ScreenVec2D,PixelPosition,ViewportPosition);
	DeprojectScreenPositionToWorld(PixelPosition.X,PixelPosition.Y,WorldLocation,WorldDirection);
	EndPoint = (WorldDirection*6000)+WorldLocation;

	//카메라에서 마우스 찍은 곳까지 라인 트레이스 
	GetWorld()->LineTraceSingleByChannel(Result,StartPoint,EndPoint,ECC_Visibility);
	return Result;
	
}

void AEnochMouseController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());

	//UI위에서는 작동 안함
	if(EGameInstance->MyPlayMenuUI->IsHoverUI())
		return;

	//마우스 아래 셀/용병 정보 얻기. 태그 기반 정보 취득
	FHitResult Result;
	GetHitResultUnderCursor(ECC_Visibility,false, Result);
	AActor* HitActor = Result.GetActor();
	
	if(Result.IsValidBlockingHit() && HitActor != nullptr)
	{
		FVector2D FieldCellLocation;
		int32 SN = 0;
		AEnochFreeLancer* HitFL = nullptr;

		if (HitActor->ActorHasTag(FName(TEXT("EnochFreeLancer"))))
		{
			HitFL = Cast<AEnochFreeLancer>(HitActor);
			Vector2D FieldLocation = HitFL->GetData()->location;
			FieldCellLocation = FVector2D(FieldLocation.x,FieldLocation.y);
			SN = HitFL->GetData()->GetSerialNumber();
		}
		else if (HitActor->ActorHasTag(FName(TEXT("EnochFieldCell"))))
		{
			auto FieldCell = Cast<AEnochFieldCell>(HitActor);
			FieldCellLocation = FieldCell->GetFieldLocation();
			if (FieldCell->HasAttachFreeLancer())
			{
				HitFL = FieldCell->GetAttachedFreeLancer();
				SN = HitFL->GetData()->GetSerialNumber();
			}
		}
		else
			return;
		
		//드래그용 정보 업데이트
		CurrentCellUnderCursor = FieldCellLocation;
		CurrentFLUnderCursor = EGameInstance->commander->GetFieldFreeLancerList(FieldCellLocation);
		
		//틱 간격마다 업데이트 되는 것을 방지. 직전 틱 정보와 같으면 업데이트 안함.
		if (PrevFLSerialNumber == SN && SN != 0)
			return;
		PrevFLSerialNumber = SN;	
		
		//빈 셀 갱신 방지.
		if (SN != 0)
		{
			//드래그 감지 위젯을 마우스 아래에 있는 용병만 활성화.
			//드래그 이미지가 앞에 있는 용병에서 날아오는 것을 방지.
			if(APrevFreeLancer != HitFL)
			{				
				if (IsValid(APrevFreeLancer))
					APrevFreeLancer->SetDragDetectOnOff(false);
				if (HitFL != nullptr)
					HitFL->SetDragDetectOnOff(true);
				APrevFreeLancer = HitFL;
			}
			
			//SN기반 정보창 업데이트
			if (HitActor->ActorHasTag(FName(TEXT("EnochFreeLancer"))) || HitActor->ActorHasTag(FName(TEXT("EnochFieldCell"))))
				EGameInstance->FlInfo->SetFLSN(SN);
		}		
	}
	else
	{
		CurrentCellUnderCursor = FVector2D().ZeroVector;
		CurrentFLUnderCursor = FLSaveData();
	}	

}

void AEnochMouseController::FreeLancerSpawn(const FHitResult& InHitResult)
{
	auto EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	FSelectedFreelancer SelectedFreeInfo = EGameInstance->SelectedFreeLancerInfo;

	//클릭한 셀/용병의 필드좌표 얻기
	FVector2D FieldLocation;
	AActor* HitActor = InHitResult.GetActor();

	//태그 기반 용병/셀 좌표 획득
	if(HitActor->ActorHasTag(FName(TEXT("EnochFreeLancer"))))
	{
		auto FieldFreeLancerLocation = Cast<AEnochFreeLancer>(HitActor)->GetData()->location;
		FieldLocation = FVector2D(FieldFreeLancerLocation.x, FieldFreeLancerLocation.y);
	}
	else if(HitActor->ActorHasTag(FName(TEXT("EnochFieldCell"))))
	{
		FieldLocation = Cast<AEnochFieldCell>(HitActor)->GetFieldLocation();
	}
	
	//적 진영에 스폰 금지.
	if (FieldLocation.Y >= 5)
		return;

	//선택된 용병이 필드가 아니고, 필드위에 이미 액터가 있으면, 해당 용병 선택 후 리턴	
	if (SelectedFreeInfo.SelectedPanel != PanelType::Field)
	{
		//용병 or (용병을 가진 셀인경우)
		if (HitActor->ActorHasTag(FName(TEXT("EnochFreeLancer"))) ||
			(HitActor->ActorHasTag(FName(TEXT("EnochFieldCell"))) && Cast<AEnochFieldCell>(HitActor)->HasAttachFreeLancer()))
			return;		
	}
	//현재 선택된 용병이 없으면 리턴. 위에랑 겹치는데 필드 위에 용병이 없는 경우 염두해서 놔둠.
	if (SelectedFreeInfo.SelectedPanel == PanelType::None)
	{
		return;
	}
	//인벤 - 스폰
	if (SelectedFreeInfo.SelectedPanel == PanelType::InvenPanel)
	{
		//인벤->필드 정보 업데이트 및 필드 스폰. 슬롯 이미지 업데이트
		//실패하면 선택된 용병 해제 안함.
		if (!EGameInstance->commander->DisposeFLInvenToFieldWrap(FieldLocation, SelectedFreeInfo.SlotNum))
			return;
	}
		//상점 - 구매 후 스폰
	else if (SelectedFreeInfo.SelectedPanel == PanelType::RecruitPanel)
	{
		//구매, 필드 스폰, 슬롯 이미지 업데이트.
		//실패하면 선택된 용병 해제 안함.
		if (!EGameInstance->commander->BuyDisposeFLToFieldWrap(SelectedFreeInfo.SlotNum, FieldLocation))
			return;
	}
		//필드 - 위치 변경 혹은 이동
	else if (SelectedFreeInfo.SelectedPanel == PanelType::Field)
	{
		//같은 곳을 또 클릭하면 무시
		if (SelectedFreeInfo.FieldPos == FieldLocation)
			return;

		//선택된 용병이 필드면 스왑 혹은 이동
		EGameInstance->commander->MoveFLInsideFieldWrap(SelectedFreeInfo.FieldPos, FieldLocation);
	}
	//선택된 용병 해제
	EGameInstance->DeselectedFLUpdate();
}

//클릭시 선택한 칸에 Melee2를 하나 생성. 생성 테스트 로직을 둘데가 없어서 일단 마우스클릭에 붙여놨음
void AEnochMouseController::OnClick()
{	
	auto EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());

	//UI위에서는 작동 안함
	if(EGameInstance->MyPlayMenuUI->IsHoverUI())
		return;

	//클릭결과 얻기
	FHitResult Result;
	GetHitResultUnderCursor(ECC_Visibility, false, Result);
	if ((!Result.IsValidBlockingHit()) || !(Result.GetActor()->ActorHasTag(FName(TEXT("EnochFreeLancer")))))
		return;
	//스폰
	FreeLancerSpawn(Result);
}
//전투 및 AI 테스트용으로 사용할 함수
void AEnochMouseController::StartFight()
{
	if (fightSimulator) return;

	auto world = GetWorld();
	if (!world) return;
	world->SpawnActor<AEnochFight>();
}

//전투 및 AI 테스트용으로 사용할 함수
void AEnochMouseController::EndFight()
{
	//if (!fightSimulator) return;
	//fightSimulator->Destroy();
	//fightSimulator = nullptr;
}

//필드에서 오브젝트를 제거하는 루틴 테스트용
void AEnochMouseController::OnRightClick()
{
	//클릭결과 얻기
	FHitResult Result;
	GetHitResultUnderCursor(ECC_Visibility, false, Result);
	if (!Result.IsValidBlockingHit()) return;

	//클릭한 필드 얻기
	auto fieldCell = (AEnochFieldCell*)Result.GetActor();

	//클릭한 필드 위의 프리랜서 얻기
	auto FreeLancer = fieldCell->GetAttachedFreeLancer();

	//프리랜서 제거(반납). FieldCell로부터의 Detach는 Release함수 내부에서 자동수행
	if (FreeLancer)
	{
		FreeLancer->Release();
	}
}
