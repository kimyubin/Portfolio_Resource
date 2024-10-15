// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MontisGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MONTIS_API UMontisGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UMontisGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;
public:
	UFUNCTION(BlueprintCallable)
	/** 월드 시작시 초기화 담당*/
	void LevelInitializer();

	/** 월드 이동시 정리 담당*/
	UFUNCTION(BlueprintCallable)
	void LevelDisposer();

	/** MontisPlayData 객체 초기화 및 리턴*/
	UFUNCTION(BlueprintCallable)
	class UMontisPlayData* GetMontisPlayData();

	UFUNCTION(BlueprintCallable)
	class UMontisGameUIsHandler* GetMontisGameUIsHandler();

	/** 레벨간 유지되어야하는 데이터 저장용 객체.*/	
	UPROPERTY()
	class UMontisPlayData* MontisPlayData;

	/** 게임 UI 핸들링 객체 */
	UPROPERTY()
	class UMontisGameUIsHandler* MontisGameUIsHandler;

	/** 레벨간 유지되어야하는 데이터 저장용 객체용 클래스.*/	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<UMontisPlayData> MontisPlayDataClass;
};
