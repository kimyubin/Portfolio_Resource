// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "KuGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class KKUMIGI_API AKuGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	virtual void HandleBeginPlay() override;

	/**
	 * 모든 액터의 BeginPlay() 호출 이후 호출됨.
	 * HandleBeginPlay()에서 모든 액터의 BeginPlay() 호출 후, HandlePostBeginPlay() 호출.  
	 */
	virtual void HandlePostBeginPlay();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};


class FKuWorldDelegates
{
public:
	DECLARE_MULTICAST_DELEGATE(FOnPrevLevelBeginPlay);	
	/** 레벨 시작 시 호출됩니다. 액터 비긴 플레이 이전에 브로드캐스팅됩니다. */
	static FOnPrevLevelBeginPlay OnPrevLevelBeginPlay;
	
	DECLARE_MULTICAST_DELEGATE(FOnPostLevelBeginPlay);
	/** 레벨 시작 시 호출됩니다. 액터 비긴 플레이 직후에 브로드캐스팅됩니다. */
	static FOnPostLevelBeginPlay OnPostLevelBeginPlay;

	/** 레벨이 종료될 때 호출됩니다. */
	DECLARE_MULTICAST_DELEGATE(FOnLevelUninitialize);
	static FOnLevelUninitialize OnLevelUninitialize;
	
};

