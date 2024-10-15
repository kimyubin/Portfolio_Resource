// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnochGameModeBase.h"
#include "EnochGameModeBasePlay.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API AEnochGameModeBasePlay : public AEnochGameModeBase
{
	GENERATED_BODY()
public:
	AEnochGameModeBasePlay();

	/**결과 화면용. Enochfight에서 받아옴*/	
	void SetFightResult(const GameResult& InResult);
	GameResult GetFightResult(){return FightResult;}

	/** 결과 송출용 UI widget*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UMG Game")
	TSubclassOf<UUserWidget> ResultWidgetClass;
	
protected:
	virtual void SubBeginPlay() override;
	/** 결과 화면 위젯 생성용*/
	UPROPERTY()
	UUserWidget* ResultUI;
	/** 결과 저장*/
	UPROPERTY()
	GameResult FightResult;

};
