// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActorInterface.h"
#include "GameFramework/Actor.h"
#include "OmniActor.generated.h"

/** 모든 액터 상위 클래스. 공용 ID 등 관리 */
UCLASS()
class OMNIBUS_API AOmniActor : public AActor, public IOmniActorInterface
{
	GENERATED_BODY()

public:
	AOmniActor();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void   SetOmniID() override;
	virtual uint64 GetOmniID() override;

protected:
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint64 MyOmniID = 0;
};
