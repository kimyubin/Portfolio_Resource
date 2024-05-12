// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActorInterface.h"
#include "GameFramework/Actor.h"
#include "OmniActor.generated.h"

class AOmnibusRoadManager;
class UOmnibusUIsHandler;
class UOmnibusPlayData;
class UOmnibusGameInstance;
/** 모든 액터 상위 클래스. 공용 ID 등 관리 */
UCLASS()
class OMNIBUS_API AOmniActor : public AActor, public IOmniActorInterface
{
	GENERATED_BODY()

	friend class AOmnibusGameStateBase;
public:
	AOmniActor();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;
	
	/**
	 * 로드되는 모든 액터의 BeginPlay()가 호출된 다음, 실행됨.
	 * 액터 초기화 순서가 필요한 경우에 사용함.
	 */
	virtual void PostBeginPlay();

public:
	virtual void Tick(float DeltaTime) override;

private:
	virtual void SetOmniID() override;

public:
	virtual uint64 GetOmniID() override;

protected:
	/** z축 고정. */
	UFUNCTION(BlueprintCallable, Category = PostEditMove)
	void SetZ_Axis();

	/** 게임 인스턴스 및 시스템 객체 */
	UFUNCTION(BlueprintCallable)
	UOmnibusGameInstance* GetOmniGameInstance() const;
	UFUNCTION(BlueprintCallable)
	UOmnibusPlayData* GetOmnibusPlayData() const;
	UFUNCTION(BlueprintCallable)
	UOmnibusUIsHandler* GetOmnibusUIsHandler() const;
	UFUNCTION(BlueprintCallable)
	AOmnibusRoadManager* GetOmnibusRoadManager() const;

	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint64 MyOmniID = 0;
};
