// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OmnibusGameInstance.generated.h"

class UOmniTimeManager;
class UOmniPathFinder;
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
	/**
	 *	GameState의 HandlePostBeginPlay()에서 호출됨.
	 *	레벨 액터 로드 후, 레벨 초기화 진행.
	 *
	 *	@see AOmnibusGameStateBase::HandlePostBeginPlay()
	 */
	UFUNCTION(BlueprintCallable)
	void LevelInitializer();

	UFUNCTION(BlueprintCallable)
	void LevelUninitializer();

	/** OmnibusPlayData 객체 초기화 및 리턴*/
	UFUNCTION(BlueprintCallable)
	UOmnibusPlayData* GetOmnibusPlayData() const;

	UFUNCTION(BlueprintCallable)
	UOmnibusUIsHandler* GetOmnibusUIsHandler() const;

	UFUNCTION(BlueprintCallable)
	AOmnibusRoadManager* GetOmnibusRoadManager() const;

	UFUNCTION(BlueprintCallable)
	UOmniPathFinder* GetOmniPathFinder() const;

	UFUNCTION()
	UOmniTimeManager* GetOmniTimeManager() const;

	/**
	 *  모든 게임플레이 레벨에서 OmnibusRoadManager에 의해 호출되어 초기화 되어야함.
	 *  아닐 경우 경고 후 종료.
	 * @param InRoadManager : 해당 레벨에서의 로드 매니저 
	 */
	UFUNCTION(BlueprintCallable)
	void SetOmnibusRoadManager(AOmnibusRoadManager* InRoadManager);

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
	TWeakObjectPtr<AOmnibusRoadManager> OmnibusRoadManager;

	UPROPERTY()
	UOmniPathFinder* OmniPathFinder;

	UPROPERTY()
	UOmniTimeManager* OmniTimeManager;
};

class FOmniWorldDelegates
{
public:
	/** 레벨이 시작 시 호출됩니다. 로드되는 액터가 초기화 완료된 후에 작동합니다. */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelInitialize, UOmnibusGameInstance*);
	static FOnLevelInitialize OnLevelInitialize;

	/** 레벨이 종료될 때 호출됩니다. */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUninitialize, UOmnibusGameInstance*);
	static FOnLevelUninitialize OnLevelUninitialize;
	
};
