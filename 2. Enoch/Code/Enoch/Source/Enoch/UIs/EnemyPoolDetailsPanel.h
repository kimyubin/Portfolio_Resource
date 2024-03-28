// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "EnemyPoolDetailsPanel.generated.h"

class USelectEnemyPoolUI;
/**
 * 
 */
UCLASS()
class ENOCH_API UEnemyPoolDetailsPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GradeNumText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PoolNumText;
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* EPScrollUniGridPanel;
	/** 자식 슬롯 보관용*/
	UPROPERTY(BlueprintReadOnly)
	TArray<UUserWidget*> ChildrenSlots;
	
	//엔진에서 사용할 위젯 클래스를 가져옴
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DetailSlot")
	TSubclassOf<UUserWidget> DetailSlotClass;
	
	/** 해당 패널이 속해있는 enemypoolUI 포인터*/
	UPROPERTY(BlueprintReadOnly)
	USelectEnemyPoolUI* MyOuterSelectEnemyPoolUI;

	void SetSelectEnemyPoolUI(USelectEnemyPoolUI* InSelectUIPtr);
	void UpdateDetailData(const int& PoolNum, int GradeNum);
	/** 적풀 최종 선택*/
	UFUNCTION(BlueprintCallable)
	void SelectEnemyPool();
	UFUNCTION(BlueprintCallable)
	void RerollButtonDown();
private:
	int tempPoolNum;
	int tempGradeNum;	
};
