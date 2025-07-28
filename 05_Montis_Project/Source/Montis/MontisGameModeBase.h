// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MontisGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class MONTIS_API AMontisGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMontisGameModeBase();
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);
	
public:

	
	/** 엔진에서 사용할 위젯 클래스를 가져옴 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartWidgetClass;
	
	/**
	 * 레벨 시작시 사용될 위젯UI
	 * 월드 별 초기 UI를 지정함
	 */
	UPROPERTY()
	UUserWidget* CurrentWidget;

};
