// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelBuilding.h"
#include "CitadelRampart.generated.h"

UCLASS()
class CITADELFOREVER_API ACitadelRampart : public ACitadelBuilding
{
	GENERATED_BODY()
	
public:	
	ACitadelRampart();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void TrackCursorForPreview(const FVector& InLocation, AActor* InHitActor) override;

	/** 타워 설치 위치*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* TowerInstallPointComponent;
	
protected:
	/** 보유 중인 타워*/
	TWeakObjectPtr<class ACitadelTower> MyInstalledTower;
	 
public:
	void InstallTowerOnRampart(class ACitadelTower* InInstallTargetTower);
	FVector GetInstallLocation() const;
	//타워 소유 여부 확인
	bool HasTower() const;
};
