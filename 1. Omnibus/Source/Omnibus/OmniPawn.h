// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActorInterface.h"
#include "GameFramework/Pawn.h"
#include "OmniPawn.generated.h"

/** 모든 폰 객체 상위 클래스. 공용 ID 등 관리 */
UCLASS()
class OMNIBUS_API AOmniPawn : public APawn, public IOmniActorInterface
{
	GENERATED_BODY()

public:
	AOmniPawn();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	virtual void SetOmniID() override;

public:
	virtual uint64 GetOmniID() override;

protected:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint64 MyOmniID = 0;
};
