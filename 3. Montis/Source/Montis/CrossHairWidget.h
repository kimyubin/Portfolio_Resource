// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossHairWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MONTIS_API UCrossHairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCrossHairWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void PlayAimingAnim(bool IsAim);
	void PlayJumpAnim(bool IsJump);
	void PlayRunAnim(bool IsRun);

	void SetLineDistanceFromCenter(double Distance);

	void SetCameraDistanceAndFOV(const float& InArmLength, const float& InFOV);

	/** 크로스 헤어 기본 위치 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double DefaultTargetPosition;
	/** 조준시 크로스 헤어 벌어지는 위치*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double AimTargetPosition;
	/** 점프시 크로스 헤어 벌어지는 위치*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double JumpTargetPosition;
	/** 달릴시 크로스 헤어 벌어지는 위치*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double RunTargetPosition;

	/** 크로스 헤어 움직이는 속도. 델타값에 곱해줌.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double LineMovementSpeed;

	/** 직전 프레임에서 크로스헤어 위치. 크로스 헤어 4개의 대표값을 가져오기 위해 정의함. */
	double CurrentPosition;

	/** 조준 상태인지 확인*/
	bool IsAimAnimRun;

	/** 시야각. 테스트할 때 화면에 표기되는 용도*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Test_FOV_Value;
	/** 카메라 붐의 거리. 테스트할 때 화면에 표기되는 용도*/
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Test_Distance_Value;
	
	UPROPERTY(meta = (BindWidget))
	UImage* Line_Up;
	UPROPERTY(meta = (BindWidget))
	UImage* Line_Down;
	UPROPERTY(meta = (BindWidget))
	UImage* Line_Left;
	UPROPERTY(meta = (BindWidget))
	UImage* Line_Right;
};
