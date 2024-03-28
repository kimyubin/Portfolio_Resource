// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UniformSlot.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Enoch/EnochDragDropOP.h"
#include "Enoch/EnochMouseController.h"
#include "Kismet/GameplayStatics.h"

bool UUniformSlot::bHoverUniformSlot = false;
void UUniformSlot::NativeConstruct()
{
	HoverBackGround->SetVisibility(ESlateVisibility::Hidden);
	LvText->SetVisibility(ESlateVisibility::Hidden);
	NameText->SetVisibility(ESlateVisibility::Hidden);
	
}

void UUniformSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	MouseHover();
}
void UUniformSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	MouseUnHover();
}
FReply UUniformSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;
	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	//드래그 이미지 설정
	Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI->DragImage->SetDragImg(MyFreeLancerTID);
	//필드 스폰 및 용병 회수를 위해 dragDected가 아닌 버튼 다운 구현
	if (MyParentPanelName == PanelType::InvenPanel)
	{
		DragInvenSlot();
	}
	else if (MyParentPanelName == PanelType::RecruitPanel)
	{
		DragRecruitSlot();
	}
	
	Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UUniformSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	//슬롯에 용병이 없으면 드래그 안함
	if(MyFreeLancerTID==FreeLancerTemplateID::None)
		return;
	UEnochDragDropOP* DragOp = Cast<UEnochDragDropOP>(UWidgetBlueprintLibrary::CreateDragDropOperation(UEnochDragDropOP::StaticClass()));

	//이동용 이미지 설정
	DragOp->DefaultDragVisual=Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI->DragImage;

	//드랍했을 때 이전에 드래그 했던 슬롯(=현 슬롯) 이미지 변경을 위한 포인터.
	DragOp->WidgetRef = this;

	//드래그 중에는 이미지 숨김
	SlotImg->SetVisibility(ESlateVisibility::Hidden);
	
	//DDoperation에 플레이어 컨트롤러 함수를 델리케이트 바인딩
	auto EController = Cast<AEnochMouseController>(UGameplayStatics::GetPlayerController(this,0));	
	DragOp->FDeleDragDropSpawnFL.BindUFunction(EController,FName("DragDropSpawnFL"));

	OutOperation=DragOp;
}

bool UUniformSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);	
	auto SelectPanel = Cast<UEnochGameInstance>(GetGameInstance())->SelectedFreeLancerInfo.SelectedPanel;
	//인벤 슬롯에서만 드롭함.
	if (MyParentPanelName == PanelType::InvenPanel)
	{
		DropInvenSlot();		
	}
	//필드 용병이면 드랍 후 필드용병 이미지 보임
	if(SelectPanel==PanelType::Field)
	{
		auto DragFL = Cast<UEnochGameInstance>(GetGameInstance())->SelectedFreeLancerInfo.DraggedFreeLancer;
		if(DragFL!=nullptr)
		{	
			DragFL->SetImageVisualOnOff(true);			
		}		
	}
	//옮기고 있는 용병이 필드에서 출발한게 아닐 경우.
	if(SelectPanel==PanelType::InvenPanel || SelectPanel == PanelType::RecruitPanel)
	{		
		//드래그 출발했던 슬롯의 숨겨놨던 이미지 다시 보이게함.
		UEnochDragDropOP* DragOP = Cast<UEnochDragDropOP>(InOperation);
		Cast<UUniformSlot>(DragOP->WidgetRef)->SlotImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);		
	}
	

	return true;
}
void UUniformSlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//드래그 취소하면 이미지 다시 보임
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	SlotImg->SetVisibility(ESlateVisibility::SelfHitTestInvisible);	
}
void UUniformSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	MouseHover();
}
void UUniformSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	MouseUnHover();
}

void UUniformSlot::MouseHover()
{
	bHoverUniformSlot = true;
	HoverBackGround->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// 용병 INFO 갱신. 없으면 갱신안함
	if (MyFreeLancerTID == FreeLancerTemplateID::None)
		return;
	Cast<UEnochGameInstance>(GetGameInstance())->FlInfo->SetFLID(MyFreeLancerTID, MyFreeLancerLv);
}

void UUniformSlot::MouseUnHover()
{
	bHoverUniformSlot = false;
	HoverBackGround->SetVisibility(ESlateVisibility::Hidden);
}

void UUniformSlot::SetMyFreeLancerInfo(const FLSaveData& FreelancerInfo)
{
	MyFreeLancerTID = FreelancerInfo.FLTemplateID;
	MyFreeLancerLv = FreelancerInfo.FLLevel;
	//우측 하단 레벨 표시
	if(MyFreeLancerLv == 0)
	{
		LvText->SetVisibility(ESlateVisibility::Hidden);
		NameText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		LvText->SetVisibility(ESlateVisibility::HitTestInvisible);
		LvText->SetText(FText::AsNumber(MyFreeLancerLv));

		auto tmp = FreeLancerTemplate::GetFreeLancerTemplate(MyFreeLancerTID);
		NameText->SetVisibility(ESlateVisibility::HitTestInvisible);
		NameText->SetText(FText::FromString(FString(tmp->name.c_str())));
	}
		
}

void UUniformSlot::SetMat(UMaterialInterface* Material)
{
	if(Material == nullptr)
		return;
	SlotImg -> SetBrushFromMaterial(Material); 
}
void UUniformSlot::SetMyParentPanelName(const PanelType& ParentName)
{
	MyParentPanelName = ParentName;
}

void UUniformSlot::SetSelectedFLData()
{
	EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	SelectedFreelancer = EGameInstance->SelectedFreeLancerInfo;
	CommanderIns = EGameInstance->commander;
	//선택된 용병 있는지
	IsSelected = (SelectedFreelancer.IsSelected);
	//현 슬롯에 용병이 있는지
	HaveFreelancer = (MyFreeLancerTID != FreeLancerTemplateID::None);
}

void UUniformSlot::DragInvenSlot()
{
	SetSelectedFLData();
	if(!IsSelected && HaveFreelancer)
	{
		//선택 용병 및 창 업데이트
		EGameInstance->SelectedFreelancerUpdate(FSelectedFreelancer(true,PanelType::InvenPanel,SlotNum,MyFreeLancerTID,0));
	}
	else if(IsSelected && HaveFreelancer)
	{
		if(SelectedFreelancer.SelectedPanel== PanelType::Field)
		{
			EGameInstance->SelectedFreelancerUpdate(
				FSelectedFreelancer(true, PanelType::InvenPanel, SlotNum, MyFreeLancerTID, 0));
		}
	}		
	else if(IsSelected && !HaveFreelancer)
	{
		if(SelectedFreelancer.SelectedPanel== PanelType::Field)
		{
			//용병 회수
			CommanderIns->BringFLFieldToInvenWrap(SelectedFreelancer.FieldPos, SlotNum);
			EGameInstance->DeselectedFLUpdate();
		}
	}
}
void UUniformSlot::DragRecruitSlot()
{
	SetSelectedFLData();
	
	//현 슬롯에 용병이 없으면 리턴
	if(MyFreeLancerTID == FreeLancerTemplateID::None)
		return;
	//선택 용병 및 창 업데이트
	EGameInstance->SelectedFreelancerUpdate(FSelectedFreelancer(true,PanelType::RecruitPanel,SlotNum,MyFreeLancerTID,0));
}
void UUniformSlot::DropInvenSlot()
{
	SetSelectedFLData();
	if(IsSelected && HaveFreelancer)
	{
		if(SelectedFreelancer.SelectedPanel==PanelType::InvenPanel)
		{
			//선택된게 인벤 슬롯이면 슬롯끼리 자리 교체
			CommanderIns->SwapInvenSlot(SelectedFreelancer.SlotNum, SlotNum);
			//선택된 용병 선택 해제.
			EGameInstance->DeselectedFLUpdate();	
		}
	}
	else if(IsSelected && !HaveFreelancer)
	{
		switch (SelectedFreelancer.SelectedPanel)
		{
		case (PanelType::InvenPanel) :
			//선택된게 인벤슬롯이면 슬롯 자리변경
			CommanderIns->SwapInvenSlot(SelectedFreelancer.SlotNum,SlotNum);

			//선택된 용병 선택해제
			EGameInstance->DeselectedFLUpdate();
			break;
		case(PanelType::RecruitPanel):
			//상점이면 구매
			//구매 및 상점, 인벤 슬롯 업데이트 실행
			if(CommanderIns->BuyFLToInvenWrap(SelectedFreelancer.SlotNum, SlotNum))
			{
				EGameInstance->DeselectedFLUpdate();					
			}
			else
			{
				//구매실패 예약석
			}	
			break;
		case(PanelType::Field):
			//용병 회수
			CommanderIns->BringFLFieldToInvenWrap(SelectedFreelancer.FieldPos, SlotNum);
			EGameInstance->DeselectedFLUpdate();		
			break;
		case(PanelType::None):
			break;
		default:
			break;
		}
	}
}
