// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OmnibusGameInstance.generated.h"

class UOmnibusPlayData;
class UOmnibusUIsHandler;
class AOmnibusRoadManager;
class UOmnibusPlayData;
class UOmnibusUIsHandler;
class AOmnibusRoadManager;

/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmnibusGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UOmnibusGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;

public:
	UFUNCTION(BlueprintCallable)
	void LevelInitializer();

	UFUNCTION(BlueprintCallable)
	void LevelDisposer();

	/** OmnibusPlayData 객체 초기화 및 리턴*/
	UFUNCTION(BlueprintCallable)
	UOmnibusPlayData* GetOmnibusPlayData();

	UFUNCTION(BlueprintCallable)
	UOmnibusUIsHandler* GetOmnibusUIsHandler();

	UFUNCTION(BlueprintCallable)
	AOmnibusRoadManager* GetOmnibusRoadManager();

	/** 레벨간 유지되어야하는 OmnibusPlayData로 사용할 BP클래스.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<UOmnibusPlayData> OmnibusPlayDataClass;

private:
	/** 레벨간 유지되어야하는 데이터 저장용 객체.*/
	UPROPERTY()
	UOmnibusPlayData* OmnibusPlayData;

	UPROPERTY()
	UOmnibusUIsHandler* OmnibusUIsHandler;

	UPROPERTY()
	AOmnibusRoadManager* OmnibusRoadManager;
};
