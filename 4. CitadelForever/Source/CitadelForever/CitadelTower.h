// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelBuilding.h"
#include "CitadelTower.generated.h"

UCLASS()
class CITADELFOREVER_API ACitadelTower : public ACitadelBuilding
{
	GENERATED_BODY()
	
public:	
	ACitadelTower();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void TrackCursorForPreview(const FVector& InLocation, AActor* InHitActor) override;
	
	/** 주변 적 감지 후, 공격 및 애니메이션 작동*/
	UFUNCTION(BlueprintCallable)
	void SearchAndAttackEnemy();
	
protected:
	/** 설치된 성벽*/
	TWeakObjectPtr<class ACitadelRampart> MyOwnerCitadelRampart;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Anim")
	class UAnimationAsset* TowerAttackAnim;
	
public:
	class ACitadelRampart* GetOwnerRampart();
	void SetOwnerRampart(class ACitadelRampart* InRampart);

};
