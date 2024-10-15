// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EveryminoGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class EVERYMINO_API AEveryminoGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	AEveryminoGameModeBase();
	
	UFUNCTION(BlueprintCallable, Category = "Widgets")	
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);	

protected:
	virtual void BeginPlay() override;

	/**
	 * 자식 클래스에서만 구현되어야함.
	 * BeginPlay()에서 ChangeWidget 이전에 호출되어야함.
	 */
	virtual void SubBeginPlay();

	
	
	/** 엔진에서 사용할 위젯 클래스를 가져옴 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> StartWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> ActionWidgetClass;
	
	/** 레벨 시작시 사용될 위젯UI. 월드 별 초기 UI 지정. */	
	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY()
	class ABrickBoardManager* MyBoardManager;

	/** 모든 게임판이 게임 오버 되면 게임 중지*/
	UPROPERTY()
	bool AllGameOver;
};
