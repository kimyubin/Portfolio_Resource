// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "OmniMacros.h"
#include "Blueprint/UserWidget.h"
#include "OmniPlayMainUI.generated.h"


class UOmniPathFinder;
class UOmniTimeManager;
class UEditableTextBox;
class UButton;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmniPlayMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void SetSpeed(const uint32 InSpeed) const;
	UFUNCTION()
	void OnButtonGameSpeedDown();
	UFUNCTION()
	void OnButtonGameSpeedUp();
	UFUNCTION()
	void OnButtonStartPauseToggle();

	/**
	 * 게임 속도가 변경된 뒤 업데이트 됩니다.
	 * TimeManger의 OnPostGameSpeedChange 델리게이트에 바인딩됩니다.
	 */
	void UpdateSpeedUI() const;

	UFUNCTION()
	void OnSpeedTextCommit(const FText& InText, ETextCommit::Type InCommitMethod);

	float GetHourHandAngle() const;

	UOmniTimeManager* GetOmniTimerManager() const;
	UOmniPathFinder* GetOmniPathFinder() const;

	UPROPERTY(meta = (BindWidget))
	UImage* ClockBaseImg;

	UPROPERTY(meta = (BindWidget))
	UImage* ClockHandImg;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DayText;

	UPROPERTY(meta = (BindWidget))
	UButton* MinusButton;

	UPROPERTY(meta = (BindWidget))
	UButton* PlusButton;

	UPROPERTY(meta = (BindWidget))
	UButton* StartPauseToggleButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartPauseToggleText;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* SpeedEditableTextBox;


	// 디버그용 메시지
	void UpdateDebugText() const;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DebugDataText;

	// 승객 상태별 색상 안내
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FindCircleText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoPathCircleText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TravelingCircleText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArrivalCircleText;

	// 승객 수 표기
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FindCountText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoPathCountText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TravelingCountText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ArrivalCountText;

	// 상태별 승객 수
	uint64 FindCount      = 0;
	uint64 NoPathCount    = 0;
	uint64 TravelingCount = 0;
	uint64 ArrivalCount   = 0;

	/**
	 * 각 여정 상태별 승객 수를 조정합니다.
	 * 
	 * @param InState 조정할 승객의 여정 상태 
	 * @param InAdd 증감할 수량
	 */
	void UpdatePassengerCount(const EJourneyState InState, const int InAdd);

	/**
	 * 승객 상태가 변경될 때, 이전 상태 count를 감소시키고, 다음 상태 count를 증가 시킵니다.
	 * 
	 * @param InPrevState 이전 상태. 숫자 감소
	 * @param InNextState 다음 상태. 숫자 증가
	 */
	void ExchangePassengerCount(const EJourneyState InPrevState, const EJourneyState InNextState);

	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdatePassengerCountUI, EJourneyState, EJourneyState);
	static FOnUpdatePassengerCountUI OnExchangePassengerCount;
	
private:
	FDelegateHandle GameSpeedChangeHandle;

	//~=============================================================================
	// 디버그 메시지 변수 및 함수 선언
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostPathFinding, int32)

	DEBUG_TEXT_MSG_DECL(PassengersNum)
	DEBUG_TEXT_MSG_DECL(Request)
	DEBUG_TEXT_MSG_DECL(Retry)
	DEBUG_TEXT_MSG_DECL(FindPath)
	DEBUG_TEXT_MSG_DECL(NoPath)
	DEBUG_TEXT_MSG_DECL(SuccessPath)
};
