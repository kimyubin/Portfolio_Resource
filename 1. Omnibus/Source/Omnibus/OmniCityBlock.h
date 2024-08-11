// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmnibusTypes.h"
#include "OmniCityBlock.generated.h"

class AOmniPassenger;
class UWidgetComponent;
enum class ECityBlockType : uint8;
class AOmniStationBusStop;
class UTextRenderComponent;
class USplineMeshComponent;
class USplineComponent;

/**
 * 승객들의 목적지가 되는 블록.
 */
UCLASS()
class OMNIBUS_API AOmniCityBlock : public AOmniActor
{
	GENERATED_BODY()

public:
	AOmniCityBlock();
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual void PostEditMove(bool bFinished) override;
#endif // WITH_EDITOR

protected:
	virtual void BeginPlay() override;
	virtual void PostBeginPlay() override;

public:
	/** OutLine 스플라인을 따라 콜리전용 staticMesh 생성. ConstructionScript 전용. */
	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void MakeCollisionAlongSpline();
	
	/** 주변 버스정류장 탐색 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = CityBlock)
	void SearchBusStop();

	/** 무게 중심 위치. 외곽선 스플라인 포인트들의 산술평균으로 위치 지정. */
	FVector GetCentroidLocation() const;

	/** CityBlock에 대한 설명을 산술평균중심으로 계산 및 이동. */
	void SetTypeSignWidgetPosition();

	/** 블록 상태 설명 설정*/
	void SetupBlockDescription(ECityBlockType InCityBlock);

	ECityBlockType GetCurrentBlockType() const;

	/**
	 * 정류장 Index 찾기
	 * @param InBusStop 찾을 버스 정류장
	 * @return 정류장의 Index. 찾지 못하면 INDEX_NONE.
	 */
	int32 FindBusStopIdx(AOmniStationBusStop* InBusStop) const;
	bool HasBusStop(AOmniStationBusStop* InBusStop) const;
	void AddBusStop(AOmniStationBusStop* InBusStop);
	void RemoveBusStop(AOmniStationBusStop* InBusStop);
	void RemoveInvalidBusStop();

	/** 보유한 모든 버스 정류장 목록 */
	TArray<TWeakObjectPtr<AOmniStationBusStop>> GetOwnedBusStops();

	/** 자신의 정류장과 도로 건너편 정류장을 포함하는 인근 정류장 목록을 반환합니다. */
	TArray<TWeakObjectPtr<AOmniStationBusStop>> FindNearbyBusStops();

	int32 GetSectorNum() const { return SubSectorList.Num(); }
	FSubSector* GetSubSector(int32 InSectorIdx);

	/** 이웃 블록인가요 */
	bool IsNeighborBlock(AOmniCityBlock* InOther) const;

	/** 자신의 Sector와 다른 섹터가 이웃하는 지 확인합니다. */
	bool IsNeighborSector(const int32 MySectorIdx, AOmniCityBlock* InOtherBlock, const int32 InOtherSectorIdx) const;


	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USplineComponent* BlockOutLine;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UWidgetComponent* TypeSignWidgetComponent;
	
protected:
	/** 콜리전 생성용 매시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* OutLineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	UMaterial* OutLineMat;

	UPROPERTY(VisibleAnywhere, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<USplineMeshComponent>> GeneratedMeshes;
	
	UPROPERTY(VisibleAnywhere, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniStationBusStop>> OwnedBusStops;

	UPROPERTY(VisibleAnywhere, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	ECityBlockType CurrentType;

	/**
	 * 블록의 하부 구역입니다.
	 * Passenger의 구체적인 출발/목적지 역할을 합니다.
	 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FSubSector> SubSectorList;
};
