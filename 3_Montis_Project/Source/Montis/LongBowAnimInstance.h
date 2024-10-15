// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LongBowAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MONTIS_API ULongBowAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	ULongBowAnimInstance();
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void SetArmed(bool bArm);
	UFUNCTION()
	void SetAimed(bool bAim);
	UFUNCTION()
	void SetShoot(bool bShoot);
	
protected:
	/** 전투 준비 태세*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LongBow, Meta=(AllowPrivateAccess=true))
	bool IsArmed;

	/** 조준 중*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LongBow, Meta=(AllowPrivateAccess=true))
	bool IsAimed;

	/** 조준 중*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LongBow, Meta=(AllowPrivateAccess=true))
	bool IsShoot;

	
	/** 이동 속도*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LongBow, Meta=(AllowPrivateAccess=true))
	float Speed;
	
	/** 추락 중*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=LongBow, Meta=(AllowPrivateAccess=true))
	bool IsFall;
	

	
};
