// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "OmniPawn.h"
#include "GameFramework/Pawn.h"
#include "OmniVehicleBus.generated.h"

class ATextRenderActor;
struct FTransferStep;
class AOmniPassenger;
struct FBusStopDistance;
class AOmniStationBusStop;
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
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/**
	 * 스폰 후 필요한 정보를 초기화합니다.
	 * 
	 * @param InOwnerRoute 노선
	 * @param InSpawnCarType 
	 * @param InSpawnRouteDist 최초 스폰된 위치 정보를 노선의 Dist로 받습니다. 이번 정류장과 CurrentRouteDist는 이 값으로 계산된 후 초기화됩니다.
	 * @see FindThisStopIdxByDist
	 */
	void SetupSpawnInit(AOmniLineBusRoute* InOwnerRoute, const ECarType InSpawnCarType, const float InSpawnRouteDist);

	/** 정류장에 정차 후 승차 시작 */
	void StartBoarding();
	void EndBoarding();

	/** 승객 탑승 */
	FTransform EntryToBus(AOmniPassenger* InPassenger);

	/**
	 * 이 버스는 이 경로로 갈 수 있나요?
	 * @InStep과 같은 경로로 가는 다른 버스임을 판단합니다.
	 * 이번 승차 시점에서 이 버스가 해당 경로로 가고 있는지 확인합니다.(2회 이상 동일 정류장에 정차하는 것을 고려)
	 * 
	 * @param InStep 이번 운행과 비교하고 싶은 환승 경로 
	 * @return 같은 시간
	 */
	bool HasSameTransStep(const FTransferStep& InStep) const;
	
	TWeakObjectPtr<AOmniLineBusRoute> GetOwnerBusRouteWeak() const { return OwnerBusRoute; }

	/** 입력된 위치에서 가장 가까운 버스노선 스플라인의 위치를 Distance로 반환 */
	float GetNearestRouteDistance(const FVector& InLocation) const;

protected:
	/** 실질적인 주행을 담당합니다. */
	void DriveOnBusLine(const float DeltaTime);

	/** 노선에서의 현재 위치 Dist(CurrentRouteDistance)를 업데이트합니다. */
	void UpdateCurrentRouteDist(const float DeltaTime);

	/** 노선 위에서 CurrentRouteDistance의 위치를 반환합니다. */
	FVector GetCurrentRouteLocation() const;

	/** 노선 스플라인의 현재 목표를 바라보는 회전 가져오기*/
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	FRotator GetLookAtDeltaRotation(const FVector InTargetPos) const;

	/** 노선 위의 가상의 추적 대상(CurrentRouteDistance)과의 간격에 맞춰, 버스의 주행 최대 속도를 설정합니다. */
	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetBusSpeed(const FVector InTargetPos);

	USplineComponent* GetOwnerRouteSpline() const;
	float GetOwnerRouteLength() const;

	/** ThisStopIndex를 바탕으로 이번 정류장의 FBusStopDistance를 가져옵니다. */
	FBusStopDistance GetThisStopDist() const;

	int32 GetThisStopIndex() const { return ThisStopIndex; };

public:
	float GetExitSpeed() const { return CarSpec.ExitSpeed; }

	UFUNCTION(BlueprintCallable, Category = "BusVehicle")
	void SetDriveMaxSpeed(const float InMaxSpeed = 1200.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USpectatorPawnMovement* PawnMovement;

	/** 버스 외관 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BusMesh;

	/** 버스 전방 감지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* ForwardSensingBoxComponent;

	/** 버스가 소속된 노선 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AOmniLineBusRoute> OwnerBusRoute;

	/** 버스 탑승자 목록 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniPassenger>> PassengerList;

	/** 버스의 종류와 주행 관련 수치를 보관합니다. */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FCarSpec CarSpec;

	/**
	 * 노선 위의 가상의 목표와의 거리 
	 * 버스는 버스 노선 위의 가상의 목표를 따라갑니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float SteeringDistance;

	/** 이번에 내릴 정류장의 노선 내부 인덱스. */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 ThisStopIndex;

	/** 현재 추적하고 있는 노선 Spline의 지점을 나타내는 거리(distance). */
	float CurrentRouteDistance;

	/** 스플라인 위치 이동 속도. */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	float NowRouteSpeed;

	/** 정류장에 진입중 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bEnterDecelArea;

	/** 정류장 앞에 완전히 멈춰서 승하차 중 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bBoarding;

#if WITH_EDITORONLY_DATA
	// 디버그 확인용
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	double DebugBusSpeed;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<ATextRenderActor> BusLineTargetTextRender;

#endif //WITH_EDITORONLY_DATA
};
