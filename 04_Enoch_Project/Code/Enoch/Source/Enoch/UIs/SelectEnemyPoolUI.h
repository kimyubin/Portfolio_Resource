// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayMenuUIManager.h"

#include "Blueprint/UserWidget.h"
#include "SelectEnemyPoolUI.generated.h"

class UEnemyPoolPanel;
class UEnemyPoolDetailsPanel;
/**
 * 
 */
UCLASS()
class ENOCH_API USelectEnemyPoolUI : public ULevelDefaultMainUI
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolPanel* EnemyPoolPanel_0;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolPanel* EnemyPoolPanel_1;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolPanel* EnemyPoolPanel_2;
	UPROPERTY(meta = (BindWidget))
	UEnemyPoolDetailsPanel* EnemyPoolDetailsPanel;
	

	static USelectEnemyPoolUI* SelectEnemyPoolUIPtr;
	static USelectEnemyPoolUI* GetSelectEnemyPoolUIPtr(){return SelectEnemyPoolUIPtr;}
	
	
	/** 디테일 패널 업데이트
	 * @param PoolNum 풀 패널 번호
	 * @param GradeNum 등급 번호
	 */
	void UpdateDetailPanel(const int& PoolNum, int GradeNum);
	/** 각 풀 패널 번호 셋팅 및 데이터 정리, 이미지 업데이트 처리*/
	void UpdateEnemyPools();
};
