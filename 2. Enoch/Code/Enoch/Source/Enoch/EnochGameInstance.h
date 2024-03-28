// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Enoch/Common/EnochFreeLancerData.h"
#include <vector>
#include <memory>

#include "CommanderWrapper.h"
#include "EnochField.h"
#include "EnochFreeLancer.h"
#include "UIs/PlayMenuUIManager.h"
#include "UIs/FLInfoImp.h"

#include "EnochGameInstance.generated.h"

UENUM(BlueprintType)
enum class PanelType : uint8 {
	None,
    RecruitPanel,
    InvenPanel,
	Field,
};
/** 현재 선택된 슬롯과 용병 정보. 임시 데이터임
 * <p> bool isSelected, PanelType selectPanel, int slotNum, uint8 fTID, int freelancerLv
 */

USTRUCT(BlueprintType)
struct FSelectedFreelancer
{
	GENERATED_USTRUCT_BODY()
public:

	FSelectedFreelancer()
	{
		IsSelected = false;
		SelectedPanel = PanelType::None;
		SlotNum = -1;	
		FreeLancerTID = FreeLancerTemplateID::None;
		FreelancerLv = -1;
		FieldPos = FVector2D(-101.f,-101.f);
		DraggedFreeLancer=nullptr;
	};
	FSelectedFreelancer(bool isSelected, PanelType selectPanel, int slotNum, uint8 fTID, int freelancerLv)	
	{
		IsSelected = isSelected;
		SelectedPanel = selectPanel;
		SlotNum = slotNum;	
		FreeLancerTID = fTID;
		FreelancerLv = freelancerLv;
		FieldPos = FVector2D(-101.f,-101.f);
		DraggedFreeLancer=nullptr;

	};
	FSelectedFreelancer(PanelType selectPanel, int slotNum, uint8 fTID)	
	{
		IsSelected = true;
		SelectedPanel = selectPanel;
		SlotNum = slotNum;	
		FreeLancerTID = fTID;
		FreelancerLv = 0;
		FieldPos = FVector2D(-101.f,-101.f);
		DraggedFreeLancer=nullptr;
	};
	FSelectedFreelancer(FVector2D fieldPos, uint8 fTID, AEnochFreeLancer* InDragFL)
	{
		IsSelected = true;
		SelectedPanel = PanelType::Field;
		SlotNum = -1;	
		FreeLancerTID = fTID;
		FreelancerLv = 0;
		FieldPos = fieldPos;
		DraggedFreeLancer=InDragFL;
	};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsSelected;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	PanelType SelectedPanel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SlotNum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 FreeLancerTID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int FreelancerLv;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D FieldPos;
	UPROPERTY(VisibleAnywhere)
	AEnochFreeLancer* DraggedFreeLancer;
};
/**
 * 
 */
UCLASS()
class ENOCH_API UEnochGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UEnochGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;

	bool InitData();
	void UninitData();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCommanderWrapper *commander;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSoloMode;

	/** 현재 선택된 용병 정보*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSelectedFreelancer SelectedFreeLancerInfo;	
	
	/** 메인 UI 접근용*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UPlayMenuUIManager* MyPlayMenuUI;
	/** EnochFiled 접근용*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	AEnochField* MyEnochField;
	
	/** 용병 정보 UI 접근용*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UFLInfoImp* FlInfo;

	
	/**
	 * 선택 용병 정보 및 선택창 업데이트
	 * @param val FSelectedFreelancer 구조체 객체. 
	 */
	void SelectedFreelancerUpdate(FSelectedFreelancer val);

	/** 용병 선택 해제. 선택용병 정보 초기화, 용병 정보 업데이트 포함*/	
	void DeselectedFLUpdate();
	
	
	/** 필드 용병 선택 기능*/
	void LiftFreeLancerInField(const FVector2D &CellLocation, AEnochFreeLancer* InFL);
private:
	//월드 변환 할때마다 초기화 되는 거 방지
	bool IsInitDataComplete;
};

