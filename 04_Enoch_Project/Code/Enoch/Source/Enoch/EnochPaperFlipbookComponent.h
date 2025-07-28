// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "Common/EnochProjectileData.h"
#include "PaperFlipbookComponent.h"
#include "Templates/SharedPointer.h"
#include "Containers/Map.h"
#include "EnochPaperFlipbookComponent.generated.h"


/**
 * AEnochFreeLancer에서 사용할 플립북 컴포넌트. 출력만 담당하는것이 목적
 */
UCLASS()
class ENOCH_API UEnochPaperFlipbookComponent : public UPaperFlipbookComponent
{
	GENERATED_BODY()

public :
		UFUNCTION(BlueprintCallable)
		void SetFreelancer(
			UPaperFlipbook *idle,
			UPaperFlipbook *attack,
			UPaperFlipbook *move,
			UPaperFlipbook *dead,
			UPaperFlipbook *victory);
		UFUNCTION(BlueprintCallable)
		void Attack();
		UFUNCTION(BlueprintCallable)
		void Idle();
		UFUNCTION(BlueprintCallable)
		void Dead();
		UFUNCTION(BlueprintCallable)
		void Move();
		UFUNCTION(BlueprintCallable)
		void Victory();

		UFUNCTION(BlueprintCallable)
		void SetProjectile(
				UPaperFlipbook* floating,
				UPaperFlipbook* bomb
			);

		void SetProjectileCopy(UEnochPaperFlipbookComponent* other);
		UFUNCTION(BlueprintCallable)
		void Floating();

		UFUNCTION(BlueprintCallable)
		void Bomb();

		UEnochPaperFlipbookComponent(){};

		UPROPERTY(BlueprintReadWrite)
		UPaperFlipbook* AttackFlipbook;
private :
	//TSharedPtr<TMap<FString, TSharedPtr<UPaperFlipbook>>> book;
	UPROPERTY()
	TMap<int, UPaperFlipbook *> books;
	void setbook(int, bool loop = true);
};