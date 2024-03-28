// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "EnochActor.h"
#include "Common/EnochProjectileData.h"
#include "EnochProjectile.generated.h"

/**
 * 
 */

UCLASS()
class ENOCH_API AEnochProjectile : public AEnochActor
{
	GENERATED_BODY()
public:
	AEnochProjectile();
	void InitPJ();

	UFUNCTION(BlueprintCallable)
	bool isValid();


	void Bomb();
protected:
	FTimerHandle bombTimer;

	UFUNCTION()
	void BombEnd();

	EnochProjectileData* GetData();
	int SerialNumber;
	int id;
	friend class AEnochActorFactory;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
};