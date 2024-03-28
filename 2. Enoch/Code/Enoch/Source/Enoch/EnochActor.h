// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "EnochPaperFlipbookComponent.h"
#include "EnochActor.generated.h"

/**
 * 
 */
UCLASS()
class ENOCH_API AEnochActor : public AActor
{
	GENERATED_BODY()
public:
	AEnochActor();
	class AEnochActorFactory* Factory;

	UFUNCTION(BlueprintCallable, Category = "EnochActor")
	virtual void SetActive(bool active);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	/** 모든 2D 그래픽 판넬이 카메라를 바라봄*/
	void RotatorSetforCamera();

	/** 뷰포트를 향해 액터 이미지만
	 *	전체를 기울이기 위한 애니메이션/플립북/UI 모음용 컴포넌트*/
	UPROPERTY(VisibleAnywhere)
	USceneComponent* AnimGroupComponent;

	UPROPERTY(BlueprintReadWrite, Category = "FLIPBOOK")
	UEnochPaperFlipbookComponent* Flipbook;
};
