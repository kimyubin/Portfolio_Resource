// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniPawn.h"
#include "GameFramework/Pawn.h"
#include "OmniVehicleBus.generated.h"

class AOmniLineBusRoute;
class UTextRenderComponent;
class UBoxComponent;
class USplineComponent;
class USpectatorPawnMovement;
class AOmniRoad;

/** 버스 유닛 최상위 클래스 */
UCLASS()
class OMNIBUS_API AOmniVehicleBus : public AOmniPawn
{
	GENERATED_BODY()

public:
	AOmniVehicleBus();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetupSpawnInit(AOmniLineBusRoute* InOuterRoute);

protected:
	void DriveToDestination(const float DeltaTime);

	/** 노선 스플라인의 현재 목표 가져오기*/
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	FVector GetTargetPointFromRouteSpline(const float DeltaTime);

	/** 노선 스플라인의 현재 목표를 바라보는 회전 가져오기*/
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	FRotator GetRotationToTarget(const FVector InTargetPos);

	/** 추적 대상 따라가는 속도*/
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetBusSpeed(const FVector InTargetPos);

public:
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveMaxSpeed(const double InMaxSpeed = 1200.0);

	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveAcceleration(const double InAcceleration = 4000.0);

	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveDeceleration(const double InDeceleration = 12000.0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USpectatorPawnMovement* PawnMovement;

	/** 버스 외관 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BusMesh;

	/** 버스 전방 감지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ForwardSensingBoxComponent;

	/**
	 * 선회할 때 바라보는 거리.
	 * 값이 클수록 먼 거리에 있는 스플라인을 확인해서 미리 회전하고,
	 * 값이 작을 수록 바로 앞의 스플라인을 따라감.
	 * 끊긴 구간보다 값이 크면 자연스럽게 넘어감.
	 * 끊긴 구간보다 값이 작으면 마지막 지점에서 제자리 회전함.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	double SteeringDistance;

	/** 노선 위에서 움직이는 추적 대상의 이동 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	double RouteTargetMoveSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniLineBusRoute> MyBusRoute;

	/** 현재 추적하고 있는 노선 Spline의 거리(distance). beginPlay에서 가장 가까운 InputKey로 초기화됨. */
	float CurrentRouteDistance;

};
