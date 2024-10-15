// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "UObject/NoExportTypes.h"
#include "OmnibusPlayData.generated.h"

class APathVisualizator;
class AOmniLineBusRoute;
class AOmniVehicleBus;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class OMNIBUS_API UOmnibusPlayData : public UObject
{
	GENERATED_BODY()

public:
	UOmnibusPlayData();

	void Initialize();

	EOmniPlayMode GetPlayMode() const { return PlayMode; }
	void          SetPlayMode(const EOmniPlayMode InPlayMode) { PlayMode = InPlayMode; }

	UStaticMesh*  GetPlainRoadMesh(const int& InMeshIdx) const;
	UStaticMesh*  GetIntersectionFlat4WayRoadMesh(const int& InMeshIdx) const;

	UTexture2D*   GetCityBlockIcon(ECityBlockType InCityBlockType) const;

	UStaticMesh* GetBusLineMesh() const { return BusLineMesh; }
	UStaticMesh* GetPathVisualMesh() const { return PathVisualMesh; }

	TSubclassOf<AOmniLineBusRoute> GetOmniLineBusRouteClass() const { return OmniLineBusRouteClass; }
	TSubclassOf<AOmniVehicleBus> GetOmniVehicleBusClass() const { return OmniVehicleBusClass; }
	TSubclassOf<APathVisualizator> GetPathVisualizatorClass() const { return PathVisualizatorClass; }
	TSubclassOf<AOmniPassenger> GetPassengerClass() const { return PassengerClass; }

	float GetBaseSpeed() const { return BaseBusSpeed; }

	/** 차종 스펙시트 데이터 테이블을 CarSpecList로 옮깁니다. */
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ReadCarSpecSheet();

	FCarSpec GetCarSpecList(const ECarType InCarType);

	TArray<FTransferRule> GetTransferRuleList() const { return TransferRuleList; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AOmniLineBusRoute> OmniLineBusRouteClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AOmniVehicleBus> OmniVehicleBusClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AOmniPassenger> PassengerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APathVisualizator> PathVisualizatorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road", meta = (AllowPrivateAccess = "true"))
	TArray<FRoadStaticMeshSet> RoadMeshSets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CityBlock", meta = (AllowPrivateAccess = "true"))
	TArray<FCityBlockIcon> CityBlockIcons;

	/** 노선 시각화용 메시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BusLine", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* BusLineMesh;

	/** 환승 경로 시각화용 메시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PathVisualizator", meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PathVisualMesh;

	/** 차종별 스펙 데이터 */
	UPROPERTY(VisibleAnywhere, Category = "CarSpec", meta = (AllowPrivateAccess = "true"))
	TMap<ECarType, FCarSpec> CarSpecList;

	/** Import된 차종별 스펙 시트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarSpec", meta = (AllowPrivateAccess = "true"))
	UDataTable* CarSpecDataTable;

	/**
	 * 버스의 기준 속도를 지정합니다.
	 * BP에서 수정 및 전역에서 접근할 수 있도록 이곳에 등록합니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarSpec", meta = (AllowPrivateAccess = "true"))
	float BaseBusSpeed;

	/** 버스 기준 가속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CarSpec", meta = (AllowPrivateAccess = "true"))
	float BaseBusAcceleration;

	/** 환승 규칙 리스트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransferRule", meta = (AllowPrivateAccess = "true"))
	TArray<FTransferRule> TransferRuleList;

protected:
	/** 현재 플레이 중인 모드. 편집, 이동 등. */
	EOmniPlayMode PlayMode;
};
