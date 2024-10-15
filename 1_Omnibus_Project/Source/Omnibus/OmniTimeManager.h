// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "UObject/NoExportTypes.h"
#include "OmniTimeManager.generated.h"

class FRealTimerManager;
class UOmnibusGameInstance;

/**
 * RealTime(현실시간) 기반 타이머를 관리하는 매니저 입니다.
 * FTimerManager와의 구분을 위해 별칭을 사용합니다.
 * 엔진에서 사용하는 일반 타이머 매니저와 완전히 동일합니다. 
 */ 
using FOmniRealTimerManager = FTimerManager;

/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmniTimeManager : public UObject
{
	GENERATED_BODY()

public:
	UOmniTimeManager();
	void Initialize(UOmnibusGameInstance* InOmniGameInstance);
	virtual void BeginDestroy() override;
	virtual UWorld* GetWorld() const override;

	void LevelInitialize(UOmnibusGameInstance* InOmniGameInstance);
	void LevelUninitialize(UOmnibusGameInstance* InOmniGameInstance);

	/** FWorldDelegates::OnWorldTickStart 이벤트 콜백 함수. DeltaTime이 확장되기 전에 호출되기 때문에, 실제 현실 시간기반의 DeltaSec가 입력됩니다. */
	void OnWorldTickStart(UWorld* InWorld, ELevelTick InLevelTick, float InRealDeltaSeconds);
	
	/** FWorldDelegates::OnWorldPreActorTick 이벤트 콜백함수. 월드의 모든 액터보다 먼저 실행됩니다. */
	void OnWorldPreActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds);

	/** FWorldDelegates::OnWorldPostActorTick 이벤트 콜백함수. 월드 모든 액터 틱이 완료된 다음 실행됩니다. */
	void OnWorldPostActorTick(UWorld* World, ELevelTick TickType, float DeltaTime);

	/** 게임을 일시정지/시작 상태를 토글합니다. 실제 일시정지 기능이 아니라 시간확장을 0으로 만듭니다. */
	void ToggleStartPauseGameTime();

	/** 현재 레벨이 시작된 후 흐른 게임 시간. 시간 배속과 일시정지에 영향을 받습니다. */
	float GetWorldTimeSinceLevelStart() const;
	
	void GameSpeedUp();
	void GameSpeedDown();

	/**
	 * 게임 속도를 지정합니다.
	 * 사전에 지정된 속도의 인덱스로 속도를 지정합니다. 
	 * @param InStep 
	 */
	void SetGameSpeed(int32 InStep);
	float GetGameSpeed();

	float GetSecInDay() const { return SecInDay; }
	float GetNightPerDay() const { return NightPerDay; }

	float GetPassengerGenerateRate() const { return PassengerGenerateRate; }
	float GetCityBlockOpenRate() const { return CityBlockOpenRate; }
	float GetCityBlockTypeChangeRate() const { return CityBlockTypeChangeRate; }
	
	int32 GetDaysInLevel() const { return DaysInLevel; }
	/**
	 * 현재 인게임 요일을 반환합니다.
	 * @note 아직 다른 곳에서 구현이 끝나지 않아, Normal만 반환합니다.
	 */
	EDay GetTodayInLevel() const;
	double GetLevelStartTimeSecond() const { return LevelStartTimeSecond; }

	/** 목적지에 도착한 후, 다시 돌아가는 시간 간격을 반환합니다. */
	float GetJourneyRepeatIntervalTime() const;
	
	void SimpleTimer(const float InRate, TFunction<void()>&& Callback);

	FOmniRealTimerManager* GetRealTimerManager() const;
	void SetSimpleRealTimer(const float InRate, TFunction<void()>&& Callback);

	/** GameSpeed를 변경한 후 호출할 함수용 델리게이트 */
	DECLARE_MULTICAST_DELEGATE(FOnPostGameSpeedChange);
	FOnPostGameSpeedChange OnPostGameSpeedChange;

protected:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UOmnibusGameInstance> OmniGameInstance;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<float> GameSpeedList;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 CurrentGameSpeedStep;

	/** 일시중지 이전의 게임 속도 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 BeforePauseGameSpeedStep;
	
	//~=============================================================================
	// 시간 기준
	/** 하루는 몇 초인가? */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float SecInDay;

	/** 하루 중 밤이 차지하는 비율 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float NightPerDay;

	/** 승객 생성 주기(Day) */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float PassengerGenerateRate;

	/** CityBlock 개방 주기(Day) */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float CityBlockOpenRate;

	/** CityBlock 종류 변경 주기(Day) */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float CityBlockTypeChangeRate;
	
	/** 현재 레벨이 시작되고나서 경과된 일수 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 DaysInLevel;

	/** 현재 레벨이 시작된 시각. 시간 배속에 영향을 받는 게임 시간입니다. */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	double LevelStartTimeSecond;

	FDelegateHandle TickStartHandle;
	FDelegateHandle PrevActorTickHandle;
	FDelegateHandle PostActorTickHandle;
	FDelegateHandle LevelInitializeHandle;
	FDelegateHandle LevelUninitializeHandle;

	//~=============================================================================
	/** RealTime(현실시간) 기반 타이머를 관리하는 매니저 */
	TUniquePtr<FOmniRealTimerManager> RealTimerManager;

	FTimerHandle LevelStartRealHandle;

};
