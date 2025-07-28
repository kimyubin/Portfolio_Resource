// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnemyPoolPanel.h"

#include "EnemyPoolDetailsPanel.h"
#include "SelectEnemyPoolUI.h"
#include "Components/TextBlock.h"
#include "Enoch/EnochGameInstance.h"

void UEnemyPoolPanel::SetEnemyPoolData(const int& InMyPanelNum)
{
	MyPanelNum = InMyPanelNum;

	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	MyEnemyPoolCountList.clear();
	EGameInstance->commander->GetEnemyPoolFLCount(InMyPanelNum, MyEnemyPoolCountList);

	//현재 적풀 내부 용병을 등급별로 정리 후 Commander에 넘겨줌. first : Tid, second : Cound
	vector<vector<pair<uint8,int>>> MyFLByGrade = vector<vector<pair<uint8, int>>>(5, vector<pair<uint8, int>>());

	TArray<UWidget*> FLImageUniformChildren = FLImageUniformPanel->GetAllChildren();
	TArray<UWidget*> FLNumUniformChildren = FLNumUniformPanel->GetAllChildren();
	TArray<UWidget*> GradeCountChildren = GradeTotalFLNumVerticalBox->GetAllChildren();

	//이미지, 문자 초기화
	for (auto& ImageWidPtr : FLImageUniformChildren)
		ImageWidPtr->SetRenderOpacity(0);
	for (auto& NumWidPtr : FLNumUniformChildren)		
		Cast<UTextBlock>(NumWidPtr)->SetText(FText::FromString(""));
	
	GradeTotalCount = vector<int>(5, 0);
	for (int i = 0; i < MyEnemyPoolCountList.size(); ++i)
	{
		//등급별로 정리
		uint8 FreeLancerGrade = FreeLancerTemplate::GetFreeLancerTemplate(MyEnemyPoolCountList[i].first)->grade;

		MyFLByGrade[FreeLancerGrade - 1].emplace_back(MyEnemyPoolCountList[i]);
		//등급내 총합 정리
		GradeTotalCount[FreeLancerGrade - 1] += MyEnemyPoolCountList[i].second;
	}

	//등급별로 정리된 데이터를 Commander에 취합
	EGameInstance->commander->SetEnemyPoolFLbyGradeList(MyPanelNum,MyFLByGrade);
	
	//등급별 총 인원수 수정
	for (int i = 0; i < GradeTotalFLNumVerticalBox->GetChildrenCount(); ++i)
	{
		Cast<UTextBlock>(GradeCountChildren[i])->SetText(FText::AsNumber(GradeTotalCount[i]));
	}

	//등급 별로 정리한 자료를 바탕으로 이미지 및 숫자 변경
	//uniform패널이라 5단위로 정리
	for (int grd = 0; grd < MyFLByGrade.size(); ++grd)
	{
		for (int i = 0; i < MyFLByGrade[grd].size(); ++i)
		{
			//4개 초과인 경우 ...으로 표시하지 못한 용병이 있음을 표기
			if(i>=MAX_GRADE_SLOTS)
			{
				Cast<UTextBlock>(FLNumUniformChildren[((grd) * 5) + (4)])->SetText(FText::FromString("..."));
				break;
			}
			
			//등급이 5단위로 나뉘어짐. 유니폼 패널 자녀 개수보다 많아지면 다음패스
			int panelSlotNum = ((grd) * 5) + (i);
			if (panelSlotNum > FLImageUniformPanel->GetChildrenCount())
				continue;

			auto GradTID = MyFLByGrade[grd][i];
			auto Mat = GetSlotImg(GradTID.first);
			Cast<UImage>(FLImageUniformChildren[panelSlotNum])->SetBrushFromMaterial(Mat);
			Cast<UImage>(FLImageUniformChildren[panelSlotNum])->SetRenderOpacity(1);
			Cast<UTextBlock>(FLNumUniformChildren[panelSlotNum])->SetText(FText::AsNumber(GradTID.second));
			
		}
	}
	//유니폼 패널 자식 개수 초과시 리턴
}

void UEnemyPoolPanel::CallDetailPanelUpdater(int MyButtonGradeNum)
{
	USelectEnemyPoolUI::GetSelectEnemyPoolUIPtr()->UpdateDetailPanel(MyPanelNum, MyButtonGradeNum);
}