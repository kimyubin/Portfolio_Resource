// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "Enoch/Common/EnochFreeLancerData.h"
#include "Enoch/Common/Commander.h"

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Enoch/DragImage.h"
#include "Components/WidgetSwitcher.h"
#include "Enoch/LevelDefaultMainUI.h"

#include "PlayMenuUIManager.generated.h"

using namespace std;

UCLASS()
class ENOCH_API UPlayMenuUIManager : public ULevelDefaultMainUI
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	//드래그 drag이미지가 영역 밖으로 나가면 이미지 숨김.
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	
	//드래그용 이미지. 별도의 블루프린트 위젯으로 만듦.
	UPROPERTY(meta = (BindWidget))
	UDragImage* DragImage;
	
	UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* RecruitUniformGridPanel;
	UPROPERTY(meta = (BindWidget))
    class UUniformGridPanel* InvenUniformGridPanel;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* SaveSlotVerticalBox;

	// UPROPERTY(meta = (BindWidget))
 //    UImage* FreeLancerInfoWidgetSlotImg;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FailCommentText;	

	//UI를 뚫고 드롭하는 거 방지
	UPROPERTY(meta = (BindWidget))
	class UEnochFieldDropProtector* RecruitFieldDropProtector;
	UPROPERTY(meta = (BindWidget))
	class UEnochFieldDropProtector* InvenFieldDropProtector;

	// 배속 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString txtCur;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString txtSlow;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString txtFast;
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* Switcher_Buttons;
	UFUNCTION(BlueprintCallable)
	void ChangeBattleSpeed(bool fast);
	void InitBattleSpeed();

	
	FTimerHandle TimeHandler;
	/** 블루프린터에서 리롤함수 접근 기능 + 슬롯 이미지 업데이트 */
	UFUNCTION(BlueprintCallable, Category = "RecruitList")
	void RerollButtonRun();
	/** BP에서 판매 함수 접근 기능*/
	UFUNCTION(BlueprintCallable, Category = "SelectedFreeLancer")
    void SellButtonRun();
	
	/** Recruit 슬롯 초기화.<p>
	* 슬롯 번호, 용병 정보, 이미지 머티리얼 업데이트	*/
	void InitRecruitSlot();
	/** inven 슬롯 초기화.<p>
	 * 슬롯 번호, 용병 정보, 이미지 머티리얼 업데이트	*/
	void InitInvenSlot();
	void InitSaveSlot();
	/** 상점 단일 슬롯 이미지 변경. 리스트 데이터를 바탕으로 자동으로 변경
	 * @param ChangeSlot 슬롯 번호*/
	void SetRecruitSlotMat(const int& ChangeSlot);
	/** 인벤 단일 슬롯 이미지 변경. 리스트 데이터를 바탕으로 자동으로 변경
	 * @param ChangeSlot 슬롯 번호*/
	void SetInvenSlotMat(const int& ChangeSlot);

	/** 드래그 이미지 숨김*/
	void DragImageHide();
	/** 드래그 이미지 보이기*/
	void DragImageVisible();
	/**구매, 스폰 등 실패시 실패된 내용을 UI에 노출 @param FailComment 실패 사유*/
	void NoticeFailComment(const FailReason& FailComment);
	void ClearFailComment();

	/** UI위에 있으면 뒤에 용병 선택, 스폰 안되는 기능*/
	bool IsHoverUI();

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txtAllianceAlly;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* txtAllianceEnemy;

	void UpdateAllianceText(wstring ally, wstring enemy);
protected:	
};