// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KuPlayerController.generated.h"

class UKuGameInstance;
struct FInputActionValue;
class AFieldItem;
class UInputMappingContext;
class UKuInputConfig;
/**
 * 
 */
UCLASS()
class KKUMIGI_API AKuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UKuGameInstance* GetKuGameInstance() const;

public:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	void OnLeftButton(const FInputActionValue& InputValue);
	void OnRightButton(const FInputActionValue& InputValue);
	void OnInventory(const FInputActionValue& InputValue);
	void OnItemRotate(const FInputActionValue& InputValue);

	void ExecuteTrackCursor();

	/** 커서 아래로 멀티 라인 트래이스. 아래에 있는 모든 객체를 채널 기반으로 체크 */
	void GetMultiHitResultsUnderCursor(TArray<FHitResult>& InHitResults, ECollisionChannel TraceChannel);


	
	/** 입력 맵핑에 대한 환경 정보. 문맥 정보 안에는 InputActionsConfig에 담긴 InputAction이 있고, 각 InputAction의 맵핑이 담겨 있음. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	UKuInputConfig* InputActionsConfig;
	

	/** 커서 추적 전용 */
	DECLARE_DELEGATE_OneParam(FOnTrackCursor, AKuPlayerController*)
	FOnTrackCursor& GetOnTrackCursorDelegate();
private:	
	FOnTrackCursor OnTrackCursor;
};


