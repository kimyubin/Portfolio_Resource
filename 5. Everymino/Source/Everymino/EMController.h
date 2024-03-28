// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "EMController.generated.h"
DECLARE_DYNAMIC_DELEGATE_TwoParams(FInputKeysDelegate, const FKey&, Key, bool, bKeyPressed);

/**
 * 
 */
UCLASS()
class EVERYMINO_API AEMController : public APlayerController
{
	GENERATED_BODY()
public:
	AEMController();
protected :
	virtual void BeginPlay() override;	
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

public:
	/**
	 * 키인풋 런타임 바인딩용.
	 * 외부에서 키입력값을 바꿀 때 호출해서 바인딩키를 일괄 변경함.
	 */
	virtual void SetupBoardManagerInput();
	
	void SetMyBoardManager(class ABrickBoardManager* InBrickBoardManager);
	
private:
	/** 런타임 키 바인딩 함수*/	
	void PressSelectNorthKey();
	void PressSelectEastKey();
	void PressSelectSouthKey();
	void PressSelectWestKey();
	
	void PressMoveUpKey();
	void PressMoveRightKey();
	void PressMoveDownKey();
	void PressMoveLeftKey();

	void PressHardDropKey();
	
	void PressClockwiseKey();
	void PressAntiClockwiseKey();

	void ReleaseMoveUpKey();
	void ReleaseMoveRightKey();
	void ReleaseMoveDownKey();
	void ReleaseMoveLeftKey();

	/** 키 맵핑 델리게이트*/
	FInputKeysDelegate InputKeysDelegate;

	UFUNCTION()
	void PressKey(const FKey& InKey, bool bKeyPressed);
	
	/** 조작 키 데이터 넘겨주는 용*/
	UPROPERTY()
	class ABrickBoardManager* MyBoardManager;
};
