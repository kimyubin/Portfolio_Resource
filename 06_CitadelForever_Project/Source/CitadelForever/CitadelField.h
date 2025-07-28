// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FieldCell.h"
#include "GameFramework/Actor.h"
#include "CitadelField.generated.h"


/** 성채 필드*/
UCLASS()
class CITADELFOREVER_API ACitadelField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACitadelField();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// 기본 필드 판. 여기에 부가 요소들이 설치됨.
	UPROPERTY(EditAnywhere, Category = "FieldFloor")
	UStaticMeshComponent* FloorMesh;

	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<class AFieldCell>> BuildingList;

};
