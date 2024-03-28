// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelPawnMovementComponent.h"

void UCitadelPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// // 모든 것이 아직 유효한지, 이동 가능한지 확인합니다.
	// if (!PawnOwner || !UpdatedComponent || ShouldSkipUpdate(DeltaTime))
	// {
	// 	return;
	// }
	//
	// // ACollidingPawn::Tick 에 설정한 무브먼트 벡터를 구(한 뒤 소거)합니다.
	// FVector DesiredMovementThisFrame = ConsumeInputVector().GetClampedToMaxSize(1.0f) * DeltaTime * 150.0f;
	// if (!DesiredMovementThisFrame.IsNearlyZero())
	// {
	// 	FHitResult Hit;
	// 	SafeMoveUpdatedComponent(DesiredMovementThisFrame, UpdatedComponent->GetComponentRotation(), true, Hit);
	//
	// 	// 무언가에 부딛혔으면, 돌아가 봅니다.
	// 	if (Hit.IsValidBlockingHit())
	// 	{
	// 		SlideAlongSurface(DesiredMovementThisFrame, 1.f - Hit.Time, Hit.Normal, Hit);
	// 	}
	// }
};