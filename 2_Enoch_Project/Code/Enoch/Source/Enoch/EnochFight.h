// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Common/EnochSimulator.h"
#include "GameFramework/Actor.h"
#include "EnochFight.generated.h"

/*
 * 전투 루틴을 매 틱마다 자동 호출하기 위해 생성하는 액터.
 * 매 전투시마다 생성하고, 전투가 끝나면 제거해야함
 */

UCLASS()
class ENOCH_API AEnochFight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnochFight();
	static bool isSimulated() { return simulated; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void StartFight();
	void EndFight();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	static bool simulated;
	int getSpeed() { return speed; }
	void setSpeed(int speed_) { speed = speed_; }
private :
	void processLog();
	EnochSimulator Simulator;
	FTimerHandle resultTimer;
	int delay;
	int speed;	//percent
};
