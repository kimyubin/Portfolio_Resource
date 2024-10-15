// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CitadelEnemyFactory.generated.h"

UCLASS()
class CITADELFOREVER_API ACitadelEnemyFactory : public AActor
{
	GENERATED_BODY()
	
public:	
	ACitadelEnemyFactory();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/** */
	void SpawnEnemy();
	
	/** 설치된 성벽 및 타워 객체 리스트 */
	TSet<TWeakObjectPtr<class ACitadelBuilding>> EnemyBuildingList;
	
	/**
	 * 적 리스트에서 입력된 빌딩의 사거리 내에 접근한 적을 찾음
	 * @param InBuilding 기준이 되는 빌딩
	 * @param Results 리턴값. 사거리 내에 들어온 모든 적 리스트
	 */
	void SearchNearEnemyList(TWeakObjectPtr<class ACitadelBuilding> InBuilding, TArray<TWeakObjectPtr<class ACitadelBuilding>>& Results);
};
