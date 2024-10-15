// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "Blueprint/UserWidget.h"
#include "Common/EnochSimulator.h"
#include "GameFramework/GameModeBase.h"
#include "EnochGameModeBase.generated.h"

UENUM(BlueprintType)
enum class GameModeType : uint8 {
	Default=0,
	StartLevel,
	EnemyPool,
	PlayLevel,
};
UCLASS()
class ENOCH_API AEnochGameModeBase : public AGameModeBase
{	
	GENERATED_BODY()		

public:
	AEnochGameModeBase();

	//엔진에서 사용할 위젯 클래스를 가져옴
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category = "UMG Game")	
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);	

protected:
	virtual void BeginPlay() override;
	/** 서브 클래스에서만 구현되어야함.
	 * BeginPlay()에서 ChangeWidget 이전에 호출되어야함.
	 */
	virtual void SubBeginPlay();
	/**
	 * 레벨 시작시 사용될 위젯UI
	 * 월드 별 초기 UI를 지정함
	 */	
	UPROPERTY()
	UUserWidget* CurrentWidget;
	
};
