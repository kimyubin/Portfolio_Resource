// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputAction.h"

#include "Archer.generated.h"

UCLASS()
class MONTIS_API AArcher : public ACharacter
{
	GENERATED_BODY()

public:
	AArcher();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveCharacter(const FInputActionInstance& Instance);

	void Look(const FInputActionInstance& Instance);
	void AimBow(const FInputActionInstance& Instance);
	void ShootBow(const FInputActionInstance& Instance);

	/** 캐릭터를 조준하는 방향으로 고정*/
	void LockCamera(bool bLock);

	/**
	 * 테스트용 FOV, 카메라 거리 변경용 함수.
	 * 거리, 화각이 변해도, 기존에 보고있던 피사체의 크기가 동일하도록 세팅
	 * 방향키로 화각과 스프링암 길이조절. 둘은 동기화됨 
	 */
	void ChangeDistanceAndFOV(const FInputActionInstance& Instance);
	/** 암 길이를 변경했을 때, 피사체 크기를 고정하기 위한 FOV 계산*/
	double ComputeFOVForSameSubject(const double& TargetArmLength);
	/** FOV를 변경했을 때, 피사체 크기를 고정하기 위한 암 길이 계산*/
	double ComputeArmLengthForSameSubject(const double& TargetFOV);
	/** 기본 화각에서 스프링 암 길이. */
	double GetDefaultArmLengthByFOV();
	/** 조준 화각에서 스프링 암 길이. ZoomRate 곱해서 리턴함 */
	double GetZoomArmLengthByFOV();
	/** UI에 스프링 암 길이 및 카메라 화각 전달.*/
	void SetUICameraArmLengthAndFOV();
	
	/**
	 * 카메라 전환시 회전랙 생성. 전환 종류시 회전랙 제거
	 * @param IsLag true면 회전에 랙 발생, false면 제거
	 */
	void SetSmoothCameraRotation(bool IsLag);
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }

//protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** 줌할 때 목표 시야각*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	double ZoomTargetFOV;
	
	/** 줌 속도*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	double ZoomSpeed;

	/** 줌할 때, 계산된 스프링암 길이에 대한 비율. 계산된 값보다 더 짧은 길이를 위함.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	double ZoomRate;
	
	/** 소켓(카메라)의 조준시 상대위치. 조준시 캐릭터 옆으로 이동 목표 위치. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector ZoomTargetSocketOffset;
	
	/** 소켓(카메라)의 기본 상대위치. 비 조준시 기본 상태.
	 *	BP에서 수정된 SocketOffset값을 받기위해 BeginPlay에서 초기화	
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FVector DefaultSocketOffset;	

	/** 기본 상태 시야각.
	 * BP에서 수정된 FieldOfView값을 받기위해 BeginPlay에서 초기화
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	double DefaultFOV;

	/** 기본 상태 카메라 암 길이.
	 * BP에서 수정된 TargetArmLength값을 받기위해 BeginPlay에서 초기화
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	double DefaultCameraArmLength;

	bool IsAimed;
	bool IsShoot;

	/**
	 * FOV 테스트용. 방향키 누르면 true, 마우스 버튼 누르면 false
	 * tick에서 조준기능과의 충돌 방지.
	 */
	bool IsTestFOV;
};
