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

	/**
	 * 레벨에서 로드되는 모든 액터의 BeginPlay()가 호출된 이후에 실행됩니다.
	 * 액터 초기화 순서가 필요한 경우에 사용합니다.
	 * 
	 * @note 플레이 중 스폰되는 경우 발동되지 않습니다.
	 */
	virtual void PostBeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	virtual void SetOmniID() override;

public:
	virtual uint64 GetOmniID() override;

protected:
	/** 게임 인스턴스 및 시스템 객체 */
	UFUNCTION(BlueprintCallable)
	virtual UOmnibusGameInstance* GetOmniGameInstance() const override;
	UFUNCTION(BlueprintCallable)
	virtual UOmnibusPlayData* GetOmnibusPlayData() const override;
	UFUNCTION(BlueprintCallable)
	virtual UOmnibusUIsHandler* GetOmnibusUIsHandler() const override;
	UFUNCTION(BlueprintCallable)
	virtual AOmnibusRoadManager* GetOmnibusRoadManager() const override;
	UFUNCTION(BlueprintCallable)
	virtual UOmniTimeManager* GetOmniTimeManager() const override;


	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	uint64 MyOmniID = 0;
};
