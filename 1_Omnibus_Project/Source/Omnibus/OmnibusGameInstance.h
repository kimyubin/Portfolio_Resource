// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OmnibusGameInstance.generated.h"

class UOmniOnlineSubsystem;
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

public:
	UOmnibusGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void PrevLevelBeginPlay();

	/**
	 *	GameState의 HandlePostBeginPlay()에서 호출됨.
	 *	레벨 액터 로드 후, 레벨 초기화 진행.
	 *
	 *	@see AOmnibusGameStateBase::HandlePostBeginPlay()
	 */
	UFUNCTION(BlueprintCallable)
	void PostLevelBeginPlay();

	UFUNCTION(BlueprintCallable)
	void LevelEnd();

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

	UFUNCTION()
	UOmniOnlineSubsystem* GetOmnibusOnlineSubsystem() const;

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

	UPROPERTY()
	UOmniOnlineSubsystem* OmniOnlineSubsystem;

	FDelegateHandle PrevLevelBeginPlayHandle;
	FDelegateHandle PostLevelBeginPlayHandle;
	FDelegateHandle LevelEndHandle;
};

class FOmniWorldDelegates
{
public:
	DECLARE_MULTICAST_DELEGATE(FOnPrevLevelBeginPlay);
	/** 레벨 시작 시 호출됩니다. 모든 액터의 BeginPlay 이전에 브로드캐스팅됩니다.*/
	static FOnPrevLevelBeginPlay OnPrevLevelBeginPlay;

	DECLARE_MULTICAST_DELEGATE(FOnPostLevelBeginPlay);
	/** 레벨 시작 시 호출됩니다. 모든 액터의 BeginPlay, PostBeginPlay 이후에 브로드캐스팅됩니다. */
	static FOnPostLevelBeginPlay OnPostLevelBeginPlay;

	DECLARE_MULTICAST_DELEGATE(FOnLevelEnd);
	/** 레벨이 종료될 때 호출됩니다. */
	static FOnLevelEnd OnLevelEnd;
};
