// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CitadelGameUIsHandler.generated.h"

/**
 * 
 */
UCLASS()
class CITADELFOREVER_API UCitadelGameUIsHandler : public UObject
{
	GENERATED_BODY()
public:
	UCitadelGameUIsHandler();

	/**
	 * 선택한 빌딩으로 UI 변경 및 해당 빌딩 객체의 선택모드로 바꾸는 함수 호출.
	 * 빌딩 -> UI handler -> 빌딩 순으로 함수가 실행됨.
	 * todo: 순서가 한바퀴 돌아감. 헷갈릴	 
	 * 이전 빌딩과 현재 빌딩 모두 상태변경을 해야해서 UI핸들에서 처리하게 만듦
	 */
	UFUNCTION(BlueprintCallable)
	void SelectBuilding(class ACitadelBuilding* InSelectedBuilding);

	/** 빌딩 선택 해제*/
	UFUNCTION(BlueprintCallable)
	void DeselectBuilding();
protected:
	bool bSelectBuilding;
	TWeakObjectPtr<class ACitadelBuilding> SelectedBuilding;
};
