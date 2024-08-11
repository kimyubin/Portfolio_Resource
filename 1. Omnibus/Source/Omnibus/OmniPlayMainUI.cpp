// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniPlayMainUI.h"

#include "OmniAsync.h"
#include "OmnibusGameInstance.h"
#include "OmniTimeManager.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Kismet/KismetStringLibrary.h"

UOmniPlayMainUI::FOnUpdatePassengerCountUI UOmniPlayMainUI::OnExchangePassengerCount;
DEBUG_TEXT_MSG_DELEGATE_DEFINE(PassengersNum)
DEBUG_TEXT_MSG_DELEGATE_DEFINE(Request)
DEBUG_TEXT_MSG_DELEGATE_DEFINE(Retry)
DEBUG_TEXT_MSG_DELEGATE_DEFINE(FindPath)
DEBUG_TEXT_MSG_DELEGATE_DEFINE(NoPath)
DEBUG_TEXT_MSG_DELEGATE_DEFINE(SuccessPath)

void UOmniPlayMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	// 게임 속도 관련
	MinusButton->OnReleased.AddDynamic(this, &UOmniPlayMainUI::OnButtonGameSpeedDown);
	PlusButton->OnReleased.AddDynamic(this, &UOmniPlayMainUI::OnButtonGameSpeedUp);
	SpeedEditableTextBox->OnTextCommitted.AddDynamic(this, &UOmniPlayMainUI::OnSpeedTextCommit);
	StartPauseToggleButton->OnReleased.AddDynamic(this, &UOmniPlayMainUI::OnButtonStartPauseToggle);

	UpdateSpeedUI();
	GameSpeedChangeHandle = GetOmniTimerManager()->OnPostGameSpeedChange.AddUObject(this, &UOmniPlayMainUI::UpdateSpeedUI);

	// 현황 UI 관련
	FindCircleText->SetColorAndOpacity(FOmniColor::Yellow);
	NoPathCircleText->SetColorAndOpacity(FOmniColor::Red);
	TravelingCircleText->SetColorAndOpacity(FOmniColor::Green);
	ArrivalCircleText->SetColorAndOpacity(FOmniColor::Blue);

	// 승객 현재 현황
	OnExchangePassengerCount.AddUObject(this, &UOmniPlayMainUI::ExchangePassengerCount);

	// 디버그 메시지 표시
	DEBUG_TEXT_MSG_CONSTRUCT(PassengersNum)
	DEBUG_TEXT_MSG_CONSTRUCT(Request)
	DEBUG_TEXT_MSG_CONSTRUCT(Retry)
	DEBUG_TEXT_MSG_CONSTRUCT(FindPath)
	DEBUG_TEXT_MSG_CONSTRUCT(NoPath)
	DEBUG_TEXT_MSG_CONSTRUCT(SuccessPath)

	UpdateDebugText();
}

void UOmniPlayMainUI::NativeDestruct()
{
	GetOmniTimerManager()->OnPostGameSpeedChange.Remove(GameSpeedChangeHandle);

	DEBUG_TEXT_MSG_DESTRUCT(PassengersNum)
	DEBUG_TEXT_MSG_DESTRUCT(Request)
	DEBUG_TEXT_MSG_DESTRUCT(Retry)
	DEBUG_TEXT_MSG_DESTRUCT(FindPath)
	DEBUG_TEXT_MSG_DESTRUCT(NoPath)
	DEBUG_TEXT_MSG_DESTRUCT(SuccessPath)

	Super::NativeDestruct();
}

void UOmniPlayMainUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);


	// todo: NativeTick 호출 스택 경로가 길기 때문에, 다른 곳에서 대리 실행하는 방법도 고려
	const float HourAngle = GetHourHandAngle();
	ClockHandImg->SetRenderTransformAngle(HourAngle);
}

void UOmniPlayMainUI::SetSpeed(const uint32 InSpeed) const
{
	GetOmniTimerManager()->SetGameSpeed(InSpeed);
}

void UOmniPlayMainUI::OnButtonGameSpeedDown()
{
	GetOmniTimerManager()->GameSpeedDown();
}

void UOmniPlayMainUI::OnButtonGameSpeedUp()
{
	GetOmniTimerManager()->GameSpeedUp();
}

void UOmniPlayMainUI::OnButtonStartPauseToggle()
{
	GetOmniTimerManager()->ToggleStartPauseGameTime();
}

void UOmniPlayMainUI::UpdateSpeedUI() const
{
	const float GameSpeed      = GetOmniTimerManager()->GetGameSpeed();
	const FString GameSpeedStr = FString::SanitizeFloat(GameSpeed);
	SpeedEditableTextBox->SetText(FText::FromString(GameSpeedStr));

	FString ToggleText = "||";
	if (GetOmniTimerManager()->GetGameSpeed() == 0)
		ToggleText = ">";

	StartPauseToggleText->SetText(FText::FromString(ToggleText));
}

void UOmniPlayMainUI::OnSpeedTextCommit(const FText& InText, ETextCommit::Type InCommitMethod)
{
	switch (InCommitMethod)
	{
		case ETextCommit::Default: break;
		case ETextCommit::OnEnter: break;
		case ETextCommit::OnUserMovedFocus: break;
		case ETextCommit::OnCleared:
			UpdateSpeedUI();
			return;
			break;
	}
	const FString CommitString = InText.ToString();
	if (InText.ToString().IsNumeric() == false)
		return;

	const int32 ConvertInt = UKismetStringLibrary::Conv_StringToInt(CommitString);
	SetSpeed(ConvertInt);
}

float UOmniPlayMainUI::GetHourHandAngle() const
{
	const float DurationSinceStartSec = GetOmniTimerManager()->GetWorldTimeSinceLevelStart();
	const float SecInHalfDay          = GetOmniTimerManager()->GetSecInDay() / 2.0f; // todo: 변경 가능성이 없다면 추후 멤버변수로 전환.
	const float RemainderSec          = fmod(DurationSinceStartSec, SecInHalfDay);
	const float Ratio                 = RemainderSec / SecInHalfDay;

	return Ratio * 360.0f;
}

UOmniTimeManager* UOmniPlayMainUI::GetOmniTimerManager() const
{
	return GetGameInstance<UOmnibusGameInstance>()->GetOmniTimeManager();
}

UOmniPathFinder* UOmniPlayMainUI::GetOmniPathFinder() const
{
	return GetGameInstance<UOmnibusGameInstance>()->GetOmniPathFinder();
}

void UOmniPlayMainUI::UpdateDebugText() const
{
	FString DebugMsg;
	DEBUG_TEXT_MSG_UPDATE("승객 수 : ", PassengersNum);

	DEBUG_TEXT_MSG_UPDATE("길찾기 검색 : ", Request);
	DEBUG_TEXT_MSG_UPDATE("재시도 횟수 : ", Retry);

	DEBUG_TEXT_MSG_UPDATE("경로 찾기 시도 : ", FindPath);
	DEBUG_TEXT_MSG_UPDATE("경로 찾기 실패 : ", NoPath);
	DEBUG_TEXT_MSG_UPDATE("경로 찾기 성공 : ", SuccessPath);

	DebugDataText->SetText(FText::FromString(DebugMsg));
}

void UOmniPlayMainUI::UpdatePassengerCount(const EJourneyState InState, const int InAdd)
{
	switch (InState)
	{
		case EJourneyState::None:
			break;
		case EJourneyState::FindingPath:
			FindCount += InAdd;
			FindCountText->SetText(FText::FromString(FString::FromInt(FindCount)));
			break;
		case EJourneyState::NoPath:
			NoPathCount += InAdd;
			NoPathCountText->SetText(FText::FromString(FString::FromInt(NoPathCount)));
			break;
		case EJourneyState::Traveling:
			TravelingCount += InAdd;
			TravelingCountText->SetText(FText::FromString(FString::FromInt(TravelingCount)));
			break;
		case EJourneyState::ArrivalCompleted:
			ArrivalCount += InAdd;
			ArrivalCountText->SetText(FText::FromString(FString::FromInt(ArrivalCount)));
			break;
		case EJourneyState::Size:
			break;
	}
}

void UOmniPlayMainUI::ExchangePassengerCount(const EJourneyState InPrevState, const EJourneyState InNextState)
{
	UpdatePassengerCount(InPrevState, -1);
	UpdatePassengerCount(InNextState, 1);
}
