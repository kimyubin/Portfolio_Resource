// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "SelectEnemyPoolUI.h"
#include "EnemyPoolDetailsPanel.h"
#include "EnemyPoolPanel.h"

USelectEnemyPoolUI* USelectEnemyPoolUI::SelectEnemyPoolUIPtr;

void USelectEnemyPoolUI::NativeConstruct()
{
	Super::NativeConstruct();
	SelectEnemyPoolUIPtr = this;
	EnemyPoolDetailsPanel->SetSelectEnemyPoolUI(this);
	UpdateEnemyPools();	
}

void USelectEnemyPoolUI::UpdateDetailPanel(const int& PoolNum, int GradeNum)
{
	EnemyPoolDetailsPanel->UpdateDetailData(PoolNum, GradeNum);
}
void USelectEnemyPoolUI::UpdateEnemyPools()
{
	//적풀 패널에 자신의 번호를 보내고 그걸 바탕으로 각자 적풀에 있는 용병 숫자정리.
	//EnemyPoolFLbyGradeList로 정리된 데이터를 받아옴
	//슬롯 이미지, 숫자까지 변경
	EnemyPoolPanel_0->SetEnemyPoolData(0);
	EnemyPoolPanel_1->SetEnemyPoolData(1);
	EnemyPoolPanel_2->SetEnemyPoolData(2);
}
