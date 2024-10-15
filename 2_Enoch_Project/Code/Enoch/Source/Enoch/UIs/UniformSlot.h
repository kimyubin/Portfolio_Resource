// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <string>

#include "Components/Image.h"
#include "Enoch/Common/EnochFreeLancerData.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Enoch/EnochGameInstance.h"

#include "UniformSlot.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API UUniformSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    //드롭 캔슬할 떄 위에 있던 버튼에서 작동
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//드래그 도중 슬롯에 들어가거나 벗어나면 작동
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* SlotBorder;
	/**레벨 표시용*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* LvText;
	/**이름 표시용*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameText;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int SlotNum;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 MyFreeLancerTID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 MyFreeLancerLv;
		
	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PanelType MyParentPanelName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* SlotImg;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UImage* HoverBackGround;

	void SetMyFreeLancerInfo(const FLSaveData& FreelancerInfo);
	void SetMat(UMaterialInterface* MyMat);
	void SetMyParentPanelName(const PanelType& ParentName);
		
	void DragInvenSlot();
	void DropInvenSlot();
	void DragRecruitSlot();
	
	/**마우스 오버시 용병 Info창 업데이트*/
	UFUNCTION(BlueprintCallable)
	void MouseHover();
	/**UI 뒤에 있는 필드 선택 불가용*/
	UFUNCTION(BlueprintCallable)
	void MouseUnHover();

	/**UI 뒤에 있는 필드 선택 불가용*/
	static bool IsHoverUniformSlot(){return bHoverUniformSlot;};
private:
	/**UI 뒤에 있는 필드 선택 불가용. 모든 유니폼 슬롯이 공유함.*/
	static bool bHoverUniformSlot;
	UEnochGameInstance* EGameInstance;
	FSelectedFreelancer SelectedFreelancer;
	UCommanderWrapper* CommanderIns;
	bool IsSelected;
	bool HaveFreelancer;

	/** 선택된 용병 및 게임인스턴스 데이터 설정<br>
	 * 초기화시 게임 인스턴스를 초기화하면 위험<br>
	 * 나머지 데이터는 사용할 때마다 업데이트*/
	void SetSelectedFLData();
};
