// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyPoolDetailsPanel.h"

#include "Enoch/EnochGameInstance.h"
#include "EnemyPoolDetailSlot.h"
#include "SelectEnemyPoolUI.h"

void UEnemyPoolDetailsPanel::NativeConstruct()
{
	Super::NativeConstruct();
	tempPoolNum = -1;
	tempGradeNum = -1;
}
void UEnemyPoolDetailsPanel::SetSelectEnemyPoolUI(USelectEnemyPoolUI* InSelectUIPtr)
{
	MyOuterSelectEnemyPoolUI = InSelectUIPtr;
}

void UEnemyPoolDetailsPanel::UpdateDetailData(const int& PoolNum, int GradeNum)
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	
	//초기화
	ChildrenSlots.Empty();
	EPScrollUniGridPanel->ClearChildren();
	
	tempPoolNum = PoolNum;
	tempGradeNum = GradeNum;
	
	//리롤시 초기화
	if (PoolNum < 0 || GradeNum < 0)
	{		
		Cast<UTextBlock>(PoolNumText)->SetText(EN_STRING_TO_FTEXT("풀 번호"));
		Cast<UTextBlock>(GradeNumText)->SetText(EN_STRING_TO_FTEXT("등급 번호"));
		return;
	}
		
	//선택된 풀 번호/등급 갱신
	Cast<UTextBlock>(PoolNumText)->SetText(FText::AsNumber(PoolNum));
	Cast<UTextBlock>(GradeNumText)->SetText(FText::AsNumber(GradeNum));
	
	//상세 내용 보여줄 풀의 등급 데이터 가져옴
	auto EnemyNumList = EGameInstance->commander->GetEnemyListbyGrade(PoolNum,GradeNum);
	
	//슬롯 위젯 생성
	for(int i = 0;i<EnemyNumList.size();++i)
	{
		UUserWidget* DetailSlot = CreateWidget<UUserWidget>(this, DetailSlotClass);
		
		Cast<UEnemyPoolDetailSlot>(DetailSlot)->SetDetailSlot(MyOuterSelectEnemyPoolUI->GetSlotImg(EnemyNumList[i].first),EnemyNumList[i].second);
		ChildrenSlots.Add(DetailSlot);
		EPScrollUniGridPanel->AddChildToUniformGrid(DetailSlot,i/4,i%4);
	}

}
void UEnemyPoolDetailsPanel::SelectEnemyPool()
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	EnemyPoolSelectionState EPSS = EnemyPoolSelectionState();
	EPSS.SelectedPool = tempPoolNum;
	EGameInstance->commander->SetEnemyPoolSelectInfo(EPSS);
}
void UEnemyPoolDetailsPanel::RerollButtonDown()
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	//아직 소지금이 없는 상태로 시작해서 false로 시작.
	EGameInstance->commander->ReqNewEnemyPool(false);
	MyOuterSelectEnemyPoolUI->UpdateEnemyPools();
	UpdateDetailData(-1, -1);
}

