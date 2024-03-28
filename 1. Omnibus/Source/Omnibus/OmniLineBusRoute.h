// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "GameFramework/Actor.h"
#include "OmniLineBusRoute.generated.h"

/** 각 버스가 지나가는 버스 노선의 최상위 클래스. 버스와 정류장을 보유함.*/
UCLASS()
class OMNIBUS_API AOmniLineBusRoute : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmniLineBusRoute();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
};
