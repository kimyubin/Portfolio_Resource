// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldCell.generated.h"

/**
 * 셀 최상위 클래스
 */
UCLASS()
class CITADELFOREVER_API AFieldCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFieldCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 셀 모양 및 표면
	UPROPERTY(EditAnywhere, Category = "FieldCellSkin")
	UStaticMeshComponent* CellMesh;

	// 셀 위에 지어진 구조물
	UPROPERTY(EditAnywhere, Category = "FieldCell")
	TWeakObjectPtr<AActor> Building;
	

};
