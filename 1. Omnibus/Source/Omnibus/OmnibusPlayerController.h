// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "OmnibusPlayerController.generated.h"

class AOmniStationBusStop;
class UOmnibusInputConfig;
class UInputMappingContext;
class UOmnibusGameInstance;
/**
 * 
 */
UCLASS()
class OMNIBUS_API AOmnibusPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	/**
	 * 직교 카메라 환경에서 커서 아래에 있는 모든 적중 결과를 반환합니다.
	 * 
	 * @param TraceChannel 추적할 콜리전 채널
	 * @param OutHitResults 적중한 결과 배열.
	 */
	void GetMultiHitResultsUnderCursorOnOrthographic(const ECollisionChannel TraceChannel, TArray<FHitResult>& OutHitResults) const;

	//~=============================================================================
	// 입력 바인딩 함수
	
	UFUNCTION(BlueprintCallable)
	void LeftButton(const FInputActionValue& InputValue);

	UFUNCTION(BlueprintCallable)
	void RightButton(const FInputActionValue& InputValue);
	
	/** 화면 드래그 이동 */
	UFUNCTION(BlueprintCallable)
	void Drag(const FInputActionValue& InputValue);

	/** 시간 정지 */
	UFUNCTION(BlueprintCallable)
	void ToggleTimeStartPause(const FInputActionValue& InputValue);

	/** 시간 속도 증감 */
	UFUNCTION(BlueprintCallable)
	void SpeedUpDownGameTime(const FInputActionValue& InputValue);

	UFUNCTION(BlueprintCallable)
	void ToggleRouteVisibility(const FInputActionValue& InputValue);

	FVector2D GetMousePosVector2D() const;

private:
	UOmnibusGameInstance* GetOmniGameInstance() const;
	void SetInputModeGameOnly();
	void SetInputModeGameUI();

public:
	/** 입력 맵핑에 대한 환경 정보. 문맥 정보 안에는 InputActionsConfig에 담긴 InputAction이 있고, 각 InputAction의 맵핑이 담겨 있음. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UOmnibusInputConfig* InputActionsConfig;

protected:
	bool bHoverUI; //UI이 위에 있을 때 클릭 및 마우스 오버 금지용.

private:
	TWeakObjectPtr<AOmniStationBusStop> StartBusStop;
};
