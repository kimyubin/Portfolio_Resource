// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/Pawn.h"
#include "CitadelDefaultPawn.generated.h"

UCLASS()
class CITADELFOREVER_API ACitadelDefaultPawn : public APawn
{
	GENERATED_BODY()

public:
	ACitadelDefaultPawn();

protected:
	virtual void BeginPlay() override;

	/** 기준 기본값을 기반으로 값 생성. 생성자 및 블루프린트에서 수정 후 BeginPlay에서도 사용 */
	void InitSettings();
	
public:
	virtual void Tick(float DeltaTime) override;

	/** 컨트롤러가 폰에 빙의했을때 호출. 입력에 함수를 바인딩함.*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 외부 엔진 클래스에서 본 무브 컴포넌트 접근 가능하게 허가*/
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	/** 카메라 상하좌우 이동*/
	void MoveCamera(const FInputActionValue& Value);
 
	/** 카메라 각도 변경*/
	void ChangeCameraAngle(const FInputActionValue& Value);
	/** 카메라 붐 상대 피치 각도*/
	void SetCameraBoomRelativePitch(double InPitch);
	/** 카메라 줌인아웃*/
	void UpdateCameraZoom(const float DeltaTime);

	/** 액터 위치로 카메라 이동*/
	void GoToActor(const FVector& TargetPosition);

	/** 디폴트 폰 위치 변경. z축 요소 걸러낸 다음 위치 변경.*/
	void SetDefaultPawnLocation(const FVector& TargetPosition);

	/** 최소 각도 기반으로 목표 카메라 각도 산출*/
	double GetDesiredAngle() const;

	/** 최소 길이 기반으로 목표 카메라암 길이 산출*/
	float GetDesiredArmLength() const;

	/** 각도 단계 상하 제한*/
	void ClampAngleStep();
protected:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraComponent, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraComponent, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCitadelPawnMovementComponent* CitadelMovementComponent;

	/** 각도 조절 단계 수*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 AngleAdjustmentSteps;

	/** 시작 각도 단계*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 StartAngleStep;
	
	/** 최대, 최소 기울기. 위에서 수직으로 내려보는게 -90도.*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	double MaxAngle;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	double MinAngle;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MaxLength;
	
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MinLength;
		
private:
	/** 휠 움직인 횟수 저장. 현재 각도 단계*/ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	int CurrentAngleStep;
	
	/** 한 틱당 단위 각도*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	double UnitAngle;

	/** 한 틱당 단위 카메라 암 길이*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float UnitLength;

	/** 카메라 줌 동작 중인지 여부 확인*/
	bool bCameraZoom;

};
