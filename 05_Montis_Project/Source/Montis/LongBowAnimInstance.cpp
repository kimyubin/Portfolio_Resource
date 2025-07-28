// Fill out your copyright notice in the Description page of Project Settings.


#include "LongBowAnimInstance.h"

#include "Archer.h"
#include "GameFramework/CharacterMovementComponent.h"

ULongBowAnimInstance::ULongBowAnimInstance()
{
	IsArmed = false;
	IsAimed = false;
	IsFall = false;
	IsShoot = false;
	Speed = 0.f;
}

void ULongBowAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	const auto OuterPawn = TryGetPawnOwner();
    
	if (IsValid(OuterPawn))
	{
		const auto OuterArcher = Cast<AArcher>(OuterPawn);

		Speed = OuterArcher->GetVelocity().Size();
		IsFall = OuterArcher->GetCharacterMovement()->IsFalling();
		
	}

}
void ULongBowAnimInstance::SetArmed(bool bArm)
{
	IsAimed = bArm;
}

void ULongBowAnimInstance::SetAimed(bool bAim)
{
	IsAimed = bAim;
}

void ULongBowAnimInstance::SetShoot(bool bShoot)
{
	IsShoot = bShoot;
}
