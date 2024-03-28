// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMDefaultPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AEMDefaultPawn::AEMDefaultPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//카메라가 회전(컨트롤러가 회전)할 때 캐릭터가 같이 회전하는 거 방지. 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	// 카메라 암 구성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // 캐릭터가 회전할 때 카메라가 따라 회전하는 거 방지
	CameraBoom->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));
	CameraBoom->TargetArmLength = 12800.f;
	CameraBoom->bDoCollisionTest = false; // 레벨과 충돌할 때 카메라 암이 줄어드는거 방지
	CameraBoom->bEnableCameraLag = true;
	
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);
	PlayerCameraComponent->SetFieldOfView(20.f);

	PlayerCameraComponent->bUsePawnControlRotation = false; // 카메라 암이 회전할 때 같이 회전하는거 방지
		
	CameraZoomSpeed = 500;
}

void AEMDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AEMDefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEMDefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MouseWheel", this, &AEMDefaultPawn::CameraAngle);
}

void AEMDefaultPawn::CameraAngle(float Axis)
{
	if ((Controller != nullptr) && (Axis != 0.0f))
	{
		CameraBoom->TargetArmLength += -(Axis*CameraZoomSpeed);
	}
}
