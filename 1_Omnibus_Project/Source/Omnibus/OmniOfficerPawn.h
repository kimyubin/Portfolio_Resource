// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "OmniOfficerPawn.generated.h"

class USpectatorPawnMovement;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class OMNIBUS_API AOmniOfficerPawn : public APawn
{
	GENERATED_BODY()

public:
	AOmniOfficerPawn();
	virtual void OnConstruction(const FTransform& Transform) override;

	/** 기준 기본값을 기반으로 값 생성. 생성자 및 블루프린트에서 수정 후 OnConstruction에서도 사용 */
	void InitSettings();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual UPawnMovementComponent* GetMovementComponent() const override;
	float GetSpringArmLength() const;

	// 맵 이동 관련
	void MoveCameraInput(const FInputActionValue& InputValue);
	void SetDragActive(const bool bInDragActive, const FVector2D& InMousePosition);
	void DragMap(const FVector2D& InMousePosition);

	// 카메라 줌 관련	
	void ZoomCameraAtCursor(const FInputActionValue& InputValue);
	void ZoomCameraCenter(const FInputActionValue& InputValue);
	void UpdateCameraZoom(const float DeltaTime);
	float GetDesiredOrthoWidth() const;

	bool AddZoomStep(const int InZoomStepDelta);
	bool AddZoomStepByAxis(const float InZoomStepAxis);

	// 액터 위치 관련	
	void GoToActor(const FVector& TargetPosition);
	void SetDefaultPawnLocation(const FVector& TargetPosition);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpectatorPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraComponent, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraComponent, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** 최대 이동 속도입니다. 실제 최대 속도는 이 값을 기준으로 화면 너비에 따라 비례해서 정해집니다. */
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float CameraMoveSpeed;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float Acceleration;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float Deceleration;

	/** 줌 속도  */
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed;

	/** 최대 줌 단계. 줌 조절 단계 수. 0~MaxZoomStep*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 MaxZoomStep;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 StartZoomStep;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MaxOrthoWidth;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MinOrthoWidth;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	int CurrentZoomStep;

	std::vector<float> OrthoWidths;

	bool      bCameraZooming;
	bool      bUsingMouseWheelCameraZoom;    // 휠로 줌
	FVector2D PrevScrollUnderCursorLocation; // 휠 굴릴 때 커서 아래 위치 임시 저장.

	bool      bDragActive;
	FVector2D DragStartMouseLocation;        // 드래그 시작 지점 마우스 위치

	/**
	 * Passenger이 렌더링 되는 줌 스텝의 상한입니다.
	 * 줌 스텝이 이 값보다 클 경우, Passenger은 렌더링 되지 않습니다. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 PassengerVisibilityMaxStep;

	/**
	 * PassengerVisibilityMaxStep으로 계산된 렌더링 유무를 가르는 직교 너비입니다.
	 * 직교 너비가 이 값보다 클 경우, Passenger은 렌더링 되지 않습니다.
	 */
	float PassengerVisibilityOrthoWidth;
};
