// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniPawn.h"
#include "GameFramework/Pawn.h"
#include "OmniVehicleUnit.generated.h"

class AOmniLineBusRoute;
class UTextRenderComponent;
class UBoxComponent;
class USplineComponent;
class USpectatorPawnMovement;
class AOmniRoad;

/** 버스 유닛 최상위 클래스 */
UCLASS()
class OMNIBUS_API AOmniVehicleUnit : public AOmniPawn
{
	GENERATED_BODY()

public:
	AOmniVehicleUnit();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	void DriveToDestination();

	/** 목표 OmniRoad로 가기 위한 회전 가져오기*/
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	FRotator GetRotationFromOmniRoadSpline();

	UFUNCTION(BlueprintCallable)
	void BeginOverlapSensor(UPrimitiveComponent* OverlappedComp
	                      , AActor*              OtherActor
	                      , UPrimitiveComponent* OtherComp
	                      , int32                OtherBodyIndex
	                      , bool                 bFromSweep
	                      , const FHitResult&    SweepResult);

	AOmniRoad*        GetNearestOmniRoad() const;
	USplineComponent* GetTargetLane();
	AOmniRoad*        GetNextRouteRoad();

	/** 버스 루트 생성. 현재 랜덤으로 생성함. */
	void              GenerateRouteRoad();

	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveMaxSpeed(const double InMaxSpeed = 1200.0);

	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveAcceleration(const double InAcceleration = 4000.0);

	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveDeceleration(const double InDeceleration = 12000.0);

	/** !사용하지 않음. 목표까지 내비게이션으로 주행 */
	UFUNCTION(BlueprintCallable, Category = "Bus")
	void MoveToTarget(const AActor* DestActor);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<USplineComponent> CurrentLane;

	/** 현재 BusRouteRoads index.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int CurrentRouteRoadIdx;
	
	TArray<TWeakObjectPtr<AOmniRoad>> BusRouteRoads;
};
