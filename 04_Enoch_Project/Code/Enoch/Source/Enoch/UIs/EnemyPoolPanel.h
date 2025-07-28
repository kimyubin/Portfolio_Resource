// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <vector>
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/VerticalBox.h"

#include "Enoch/LevelDefaultMainUI.h"
#include "Enoch/Common/Commander.h"

#include "EnemyPoolPanel.generated.h"

class USelectEnemyPoolUI;

using namespace std;

/**
 * 
 */
UCLASS()
class ENOCH_API UEnemyPoolPanel : public ULevelDefaultMainUI
{
	GENERATED_BODY()
public:
	//적 풀 패널 내부 widget 접근용
	/** 내부에 있는 용병들 이미지*/
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* FLImageUniformPanel;
	/** 각 용병 수*/
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* FLNumUniformPanel;
	/** 등급내 총 용병수*/
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* GradeTotalFLNumVerticalBox;

	/** 등급별 표기 용병 최대 숫자. 이를 초과하는 용병은 표기 생략*/
	static constexpr int MAX_GRADE_SLOTS = 4;
	/**등급별 총 숫자*/
	vector<int> GradeTotalCount;
		
	UPROPERTY(BlueprintReadWrite)
	int MyPanelNum;
	
	/** 현재 적풀의 전체 용병 TID와 해당 용병의 수*/
	vector<pair<uint8,int>> MyEnemyPoolCountList;

	
	/** 현재 패널 번호를 바탕으로 적 풀 리스트를 받아서 정리.
	 * 풀 패널 화면 갱신.
	 * 정리한 현재 적풀 내부의 용병 숫자를 poolUI으로 보냄.
	 * @param InMyPanelNum 현재 패널 번호
	 */
	void SetEnemyPoolData(const int& InMyPanelNum);

	/** 풀 패널 내에 있는 용병 등급 클릭시 상세 정보 볼 수 있게.
	 * BP에서 버튼이랑 매치*/
	UFUNCTION(BlueprintCallable)
	void CallDetailPanelUpdater(int MyButtonGradeNum);
	
	
};
