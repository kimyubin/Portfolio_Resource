// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MontisPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MONTIS_API AMontisPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMontisPlayerController();
protected:
	virtual void BeginPlay() override;
public:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	void SetInputModeGameOnly();
	void SetInputModeGameAndUI();

	/** 커서 아래로 멀티 라인 트래이스. 아래에 있는 모든 객체를 채널 기반으로 체크 */
	UFUNCTION(BlueprintCallable)
	void GetMultiHitResultsUnderCursor(TArray<FHitResult>& OutHitResults, ECollisionChannel TraceChannel);

	/**
	 * 조준 전후 카메라 위치 변경됨에 따라,
	 * 카메라 이동 후에도, 현재 타겟을 바라보도록 컨트롤러를 회전.
	 * @param ActorLocation 캐릭터 위치. 회전 원점 역할을 함
	 * @param ChangeCameraOffset 변경할 카메라 오프셋. x값에 SpringArmLength 포함
	 * @param TraceChannel 라인 트레이스 채널
	 */
	void RotateCameraBoomToTarget(const FVector& ActorLocation, const FVector& ChangeCameraOffset, ECollisionChannel TraceChannel = ECC_Camera);

	/**
	 * 테스트 코드
	 * 현재 타겟을 조준 전후에도 카메라가 바라보도록 컨트롤러를 회전.	 
	 * 한번에 계산하기 위한 테스트 코드
	 * @param ActorTransform 조준 중인 캐릭터의 트랜스폼. 회전 원점 역할을 함
	 * @param CurrentCamOffset 기본 카메라 오프셋. x값에 SpringArmLength 포함
	 * @param TargetCameraOffset 조준시 변경되는 카메라 오프셋. x값에 SpringArmLength 포함.
	 * @param TraceChannel 라인 트레이스 채널
	 */
	UFUNCTION(BlueprintCallable)
	void TestRotateControllerTowardTargetForCamera(FTransform ActorTransform,FVector CurrentCamOffset,  FVector TargetCameraOffset, ECollisionChannel TraceChannel = ECC_Camera);

	
	/** 입력 맵핑에 대한 환경 정보. 문맥 정보 안에는 InputActionsConfig에 담긴 InputAction이 있고, 각 InputAction의 맵핑이 담겨 있음. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UMontisInputConfig* InputActionsConfig;
	
};
