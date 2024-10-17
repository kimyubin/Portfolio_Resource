// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniTimeManager.h"

#include "OmnibusGameInstance.h"

#include "UtlLog.h"
#include "UTLStatics.h"


UOmniTimeManager::UOmniTimeManager()
{
	GameSpeedList            = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
	CurrentGameSpeedStep     = 1;
	BeforePauseGameSpeedStep = 0;

	SecInDay    = 20.0f;
	NightPerDay = 5.0f;

	PassengerGenerateRate   = 1.0f;
	CityBlockOpenRate       = 2.0f;
	CityBlockTypeChangeRate = 7.0f;
	
	DaysInLevel          = 0;
	LevelStartTimeSecond = 0.0f;
}

void UOmniTimeManager::Initialize(UOmnibusGameInstance* InOmniGameInstance)
{
	Super::Initialize(InOmniGameInstance);

	RealTimerManager = MakeUnique<FOmniRealTimerManager>(InOmniGameInstance);

	// 레벨 틱 초반에 작동
	TickStartHandle     = FWorldDelegates::OnWorldTickStart.AddUObject(this, &UOmniTimeManager::OnWorldTickStart);
	PrevActorTickHandle = FWorldDelegates::OnWorldPreActorTick.AddUObject(this, &UOmniTimeManager::OnWorldPreActorTick);
	// PostActorTickHandle = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UOmniTimeManager::OnWorldPostActorTick);

	PostLevelBeginPlayHandle = FOmniWorldDelegates::OnPostLevelBeginPlay.AddUObject(this, &UOmniTimeManager::PostLevelBeginPlay);
	LevelEndHandle  = FOmniWorldDelegates::OnLevelEnd.AddUObject(this, &UOmniTimeManager::LevelEnd);
}

void UOmniTimeManager::BeginDestroy()
{
	// 객체 파괴시 델리게이트 명시적 제거. (Broadcast시 암시적 제거하지만 명시적으로 제거함.)
	FWorldDelegates::OnWorldTickStart.Remove(TickStartHandle);
	FWorldDelegates::OnWorldPreActorTick.Remove(PrevActorTickHandle);
	// FWorldDelegates::OnWorldPostActorTick.Remove(PostActorTickHandle);

	FOmniWorldDelegates::OnPostLevelBeginPlay.Remove(PostLevelBeginPlayHandle);
	FOmniWorldDelegates::OnLevelEnd.Remove(LevelEndHandle);

	UObject::BeginDestroy();
}

UWorld* UOmniTimeManager::GetWorld() const
{
	check(GetOmniGameInstance())

	if (const UOmnibusGameInstance* GameInstance = GetOmniGameInstance())
		return GameInstance->GetWorld();

	return nullptr;
}

void UOmniTimeManager::PostLevelBeginPlay()
{
	DaysInLevel          = 0;
	LevelStartTimeSecond = GetWorld()->GetTime().GetWorldTimeSeconds();
	SetGameSpeed(0);

	// 게임 시작 전 대기 시간
	if (RealTimerManager.IsValid())
	{
		RealTimerManager->SetTimer(LevelStartRealHandle, [thisWeak = TWeakObjectPtr<UOmniTimeManager>(this)]()
		{
			UT_LOG("RealTimer Start")

			UOmniTimeManager* OmniTime = thisWeak.Get();
			if (OmniTime != nullptr)
				OmniTime->SetGameSpeed(1);
		}, 1.0f, false);
	}
	else
	{
		UT_LOG("RealTimerManager is not valid.")
		SetGameSpeed(1);
	}
}

void UOmniTimeManager::LevelEnd()
{
	DaysInLevel          = 0;
	LevelStartTimeSecond = 0.0f;
	SetGameSpeed(1);

	RealTimerManager->ClearTimer(LevelStartRealHandle);
}

void UOmniTimeManager::OnWorldTickStart(UWorld* InWorld, ELevelTick InLevelTick, float InRealDeltaSeconds)
{
	if(RealTimerManager.IsValid())
		RealTimerManager->Tick(InRealDeltaSeconds);
}

void UOmniTimeManager::OnWorldPreActorTick(UWorld* InWorld, ELevelTick InLevelTick, float InDeltaSeconds)
{
	

}

void UOmniTimeManager::OnWorldPostActorTick(UWorld* World, ELevelTick TickType, float DeltaTime)
{
}

void UOmniTimeManager::ToggleStartPauseGameTime()
{
	if (CurrentGameSpeedStep > 0 && BeforePauseGameSpeedStep > 0)
		BeforePauseGameSpeedStep = 0;

	Swap(CurrentGameSpeedStep, BeforePauseGameSpeedStep);
	SetGameSpeed(CurrentGameSpeedStep);
}

float UOmniTimeManager::GetWorldTimeSinceLevelStart() const
{
	// 현재 로직은 GetWorldTimeSeconds()와 값이 같지만,
	// 추후 누적시간을 따로 계산할수도 있기 때문에, 레퍼함수 사용
	return GetWorld()->GetTime().GetWorldTimeSeconds() - LevelStartTimeSecond;
}

void UOmniTimeManager::GameSpeedUp()
{
	SetGameSpeed(CurrentGameSpeedStep + 1);
}

void UOmniTimeManager::GameSpeedDown()
{
	SetGameSpeed(CurrentGameSpeedStep - 1);
}

void UOmniTimeManager::SetGameSpeed(const int32 InStep)
{
	CurrentGameSpeedStep = FMath::Clamp(InStep, 0, GameSpeedList.Num() - 1);

	GetWorld()->GetWorldSettings()->SetTimeDilation(GetGameSpeed());
	OnPostGameSpeedChange.Broadcast();
}

float UOmniTimeManager::GetGameSpeed()
{
	UT_IF(GameSpeedList.IsValidIndex(CurrentGameSpeedStep) == false)
		return 0.0;

	return GameSpeedList[CurrentGameSpeedStep];
}

EDay UOmniTimeManager::GetTodayInLevel() const
{
	EDay ResultDay = EDay::Normal;
	int WeakDay = GetDaysInLevel() % 7;
	if(WeakDay <= 4)
		ResultDay = EDay::Normal;
	else
		ResultDay = EDay::Holiday;
	// todo: 평일로만 계산 중
	return EDay::Normal;
}

float UOmniTimeManager::GetJourneyRepeatIntervalTime() const
{
	// 임시: 반나절 후 되돌아감
	return SecInDay / 4.0f;
}

void UOmniTimeManager::SimpleTimer(const float InRate, TFunction<void()>&& Callback)
{
	FTimerHandle TempTimer;
	GetWorld()->GetTimerManager().SetTimer(TempTimer, MoveTemp(Callback), InRate, false);
}

FOmniRealTimerManager* UOmniTimeManager::GetRealTimerManager() const
{
	return RealTimerManager.Get();
}

void UOmniTimeManager::SetSimpleRealTimer(const float InRate, TFunction<void()>&& Callback)
{
	FTimerHandle TempRealTimer;
	RealTimerManager->SetTimer(TempRealTimer, MoveTemp(Callback), InRate, false);
}
