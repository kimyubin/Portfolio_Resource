// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "OmnibusPlayerController.generated.h"

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

	UFUNCTION(BlueprintCallable)
	void LeftButton(const FInputActionValue& InputValue);

	UFUNCTION(BlueprintCallable)
	void RightButton(const FInputActionValue& InputValue);
	
	// 화면 드래그 이동
	UFUNCTION(BlueprintCallable)
	void Drag(const FInputActionValue& InputValue);

	UFUNCTION(BlueprintCallable)
	void SpawnAndTrackPreviewBusStop(EBusStopType PreviewType);

	UFUNCTION(BlueprintCallable)
	void SpawnBusStop();

	UFUNCTION(BlueprintCallable)
	void CancelBuild();

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
	bool IsPreviewMode;
	bool bHoverUI;			//UI이 위에 있을 때 클릭 및 마우스 오버 금지용.
	
private:
	TWeakObjectPtr<AActor> PC_Weak_PreviewBusStop;
};
