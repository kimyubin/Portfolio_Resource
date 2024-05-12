// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmniCityBlock.generated.h"

class UWidgetComponent;
enum class ECityBlockType : uint8;
class AOmniStationBusStop;
class UTextRenderComponent;
class USplineMeshComponent;
class USplineComponent;
/**
 * 다이나믹 메시로 블럭 생성 및 주변 버스 정류장 감지
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

public:
	/** OutLine 스플라인을 따라 콜리전용 staticMesh 생성. ConstructionScript 전용. */
	UFUNCTION(BlueprintCallable, Category = CustomConstructionScript)
	void MakeCollisionAlongSpline();
	
	/** 주변 버스정류장 탐색 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category = CityBlock)
	void SearchBusStop();

	/** CityBlock에 대한 설명을 산술평균중심으로 계산 및 이동. */
	void CalculateTypeSignPosition();

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

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USplineComponent* BlockOutLine;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UWidgetComponent* TypeSignWidgetComponent;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* PlacedMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	UMaterial* PlacedMat;

	UPROPERTY(VisibleAnywhere, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<USplineMeshComponent>> GeneratedMeshes;
	
	UPROPERTY(VisibleAnywhere, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	TArray<TWeakObjectPtr<AOmniStationBusStop>> OwnedBusStops;

	UPROPERTY(VisibleAnywhere, Category = CityBlock, meta = (AllowPrivateAccess = "true"))
	ECityBlockType CurrentType;
	
};
