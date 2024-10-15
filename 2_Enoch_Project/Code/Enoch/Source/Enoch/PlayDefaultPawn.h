// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayDefaultPawn.generated.h"

UCLASS()
class ENOCH_API APlayDefaultPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayDefaultPawn();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* DefaultPawnCamera;
	/** 카메라붐 중심으로 회전. 회전 움직임 smooth 조절*/
	UPROPERTY(EditAnywhere)
	class USpringArmComponent* CameraBoom;
	/** 최대 기울기. 위에서 수직으로 내려보는게 -90도.*/
	UPROPERTY(EditAnywhere)
	float MaxAngle;
	/** 각도 조절 단계 수.*/
	UPROPERTY(EditAnywhere)
	int TotalNumberOfSteps;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** 마우스 휠 감지 및 휠 회전 수를 저장. */
	void ZoomInOut(float Axis);

	/**
	 * 카메라 기울기 값을 용병용으로 변환해서 반환. 
	 * 수직으로 바라볼때 용병이 완전히 90도로 누워서 바닥에 파고드는 현상 방지. 0.01f을 뺌.
	 * @return Pitch만 -0.01도
	 */
	static FRotator GetCameraArmRotate(){return FRotator(CameraArmRotate.Pitch+0.01f,CameraArmRotate.Yaw,CameraArmRotate.Roll);}

private:
	/** 휠 움직인 횟수 저장. Min:1/Max:20 각도를 20단계로 쪼갬*/ 
	UPROPERTY(VisibleAnywhere)
	int TargetAngleStep;
	/** 한 틱당 단위 각도*/
	UPROPERTY(VisibleAnywhere)
	float UnitAngle;
	/** 카메라 앵글 값*/
	static FRotator CameraArmRotate;
};
