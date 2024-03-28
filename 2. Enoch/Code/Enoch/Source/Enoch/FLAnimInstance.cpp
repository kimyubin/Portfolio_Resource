// Fill out your copyright notice in the Description page of Project Settings.


#include "FLAnimInstance.h"

UFLAnimInstance::UFLAnimInstance()
{
	state = FreeLancerState::Idle;
	actSpeed = 1.0f;
}

void UFLAnimInstance::SetState(FreeLancerState stateIn, float speed)
{ 
	state = stateIn;
	actSpeed = speed;
}