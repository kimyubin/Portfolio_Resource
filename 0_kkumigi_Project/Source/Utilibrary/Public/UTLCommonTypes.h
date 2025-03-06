// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilibrary.h"
#include "UObject/NoExportTypes.h"
#include "UTLCommonTypes.generated.h"

struct FInputActionValue;
class USpringArmComponent;
/**
 * 
 */
UCLASS()
class UTILIBRARY_API UUTLCommonTypes : public UObject
{
	GENERATED_BODY()
	
};

/**
 * 스프링암 컴포넌트에 대한 제어 기능을 모아 놓은 컴포넌트입니다.
 * 입력을 기반으로, 스프링암의 각도와 카메라 거리를 컨트롤하며,
 * Tick 동안 움직임을 보간해 부드러운 전환이 가능합니다.
 */
UCLASS()
class UTILIBRARY_API USpringArmControlComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USpringArmControlComponent();

	void SetDefaultConfig(USpringArmComponent* InSpringArmComponent
	                    , const FVector& InCameraDeadZoneMin
	                    , const FVector& InCameraDeadZoneMax
	                    , const uint8 InAngleAdjustmentSteps
	                    , const uint8 InStartAngleStep
	                    , const float InMaxAngle
	                    , const float InMinAngle
	                    , const float InMaxLength
	                    , const float InMinLength
	                    , const bool InbWheelUpToCamUp = true);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR

	virtual void BeginPlay() override;

	/**
	 * 입력된 값을 바탕으로 파생된 값을 계산합니다.
	 * BeginPlay에서 호출해야 합니다.
	 */
	void InitAngleSettings();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void UpdateLocationInDeadZone(const float DeltaTime);

	void TickArmLength(const float DeltaTime);

public:
	void ChangeCameraAngleInput(float InInputAxis);

	bool IsValid() const;

private:
	/** 스프링암 각도를 Pitch만 조절합니다. */
	void SetSpringArmRelativePitch(double InPitch);

	/** 현재 스탭에서 각도를 가져옵니다. */
	double GetDesiredAngle() const;

	/** 현재 스탭에서 스프링암 길이(카메라 거리)를 가져옵니다. */
	float GetDesiredArmLength() const;

	/** 스탭이 범위를 벗어나지 않게 클램프 처리를 합니다. */
	void ClampAngleStep();

private:
	TWeakObjectPtr<USpringArmComponent> SpringArmWeakPtr;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	bool bUsedCameraDeadZone = true;

	/** 데드존의 최소 위치. 캐릭터 중심을 기준 */
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true", EditCondition = "bUsedCameraDeadZone"))
	FVector CameraDeadZoneMin = FVector::ZeroVector;

	/** 데드존의 최대 위치. 캐릭터 중심을 기준 */
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true", EditCondition = "bUsedCameraDeadZone"))
	FVector CameraDeadZoneMax = FVector::ZeroVector;

	/** true이면 휠업으로 카메라 위치를 위로 올립니다.(시선을 아래를 향하게 됩니다.) */
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	bool bWheelUpToCamUp;

	/** 각도 조절 단계 수*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 AngleAdjustmentSteps;

	/** 시작 각도 단계*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	uint8 StartAngleStep;

	/** 최대, 최소 기울기. 위에서 수직으로 내려보는게 -90도.*/
	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MaxAngle;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MinAngle;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MaxLength;

	UPROPERTY(EditAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float MinLength;


	/** 휠 움직인 횟수 저장. 현재 각도 단계*/
	UPROPERTY(VisibleAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	int8 CurrentAngleStep;

	/** 단계별 단위 각도*/
	UPROPERTY(VisibleAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float UnitAngle;

	/** 단계별 단위 카메라 거리 */
	UPROPERTY(VisibleAnywhere, Category = CameraControlSettings, meta = (AllowPrivateAccess = "true"))
	float UnitLength;

	/** 카메라 줌 동작 중인지 여부 확인*/
	bool bCameraZoom;
};
