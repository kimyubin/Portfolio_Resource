// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Animation/AnimInstance.h"
#include "Common/EnochFreeLancerData.h"
#include "FLAnimInstance.generated.h"

/**
 * decrecated (3d 현재 쓸 생각 없음)
 */
UCLASS()
class ENOCH_API UFLAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	UFLAnimInstance();

public:
	void SetState(FreeLancerState stateIn, float speed = 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta=(AllowPrivateAccess = true))
	FreeLancerState state;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pawn, Meta = (AllowPrivateAccess = true))
	float actSpeed = 1.0f;
};