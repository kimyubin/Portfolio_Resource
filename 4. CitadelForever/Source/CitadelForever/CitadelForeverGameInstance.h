// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CitadelForeverGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CITADELFOREVER_API UCitadelForeverGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:

	UCitadelForeverGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	/** 월드 시작시 초기화 담당*/
	void LevelInitializer();

	/** 월드 이동시 정리 담당*/
	UFUNCTION(BlueprintCallable)
	void LevelDisposer();

	/** CitadelPlayData 객체 초기화 및 리턴*/
	UFUNCTION(BlueprintCallable)
	class UCitadelPlayData* GetCitadelPlayData();
	class ACitadelDefaultPawn* GetCitadelDefaultPawn();
	class ACitadelBuildingFactory* GetCitadelBuildingFactory();
	class UCitadelGameUIsHandler* GetCitadelGameUIsHandler();
protected:
	
	/** 레벨간 유지되어야하는 데이터 저장용 객체.*/	
	UPROPERTY()
	class UCitadelPlayData* CitadelPlayData;
	
	/** 디폴트 폰 대신 사용할 기본 폰. 스폰 위치 등을 제어하기 위해 별개로 구성*/
	UPROPERTY()
	class ACitadelDefaultPawn* CitadelDefaultPawn;
	
	/** 스폰 팩토리 객체*/
	UPROPERTY()
	class ACitadelBuildingFactory* CitadelBuildingFactory;

	/** 게임 UI 핸들링 객체 */
	UPROPERTY()
	class UCitadelGameUIsHandler* CitadelGameUIsHandler;
	
public:
	/** 레벨간 유지되어야하는 데이터 저장용 객체용 클래스.*/	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<class UCitadelPlayData> CitadelPlayDataClass;

};
