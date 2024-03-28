// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PulicSlotImgArr.generated.h"

UCLASS()
class ENOCH_API APulicSlotImgArr : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APulicSlotImgArr();
	/**
	* 슬롯 이미지 저장하는 배열<br>
	* 엔진 BP 프로퍼티에서 이미지 교체 가능
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProfileImg")
	TArray<UMaterialInterface*> SlotImgArr;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
