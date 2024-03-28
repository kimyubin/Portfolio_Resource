// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OmnibusGameModeBase.generated.h"

/**
 * 
 */
UCLASS()

class OMNIBUS_API AOmnibusGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOmnibusGameModeBase();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "UMG Game")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> StartWidgetClass;

	UPROPERTY()
	UUserWidget* CurrentWidget;
};
