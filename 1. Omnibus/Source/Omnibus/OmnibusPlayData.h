// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmnibusTypes.h"
#include "UObject/NoExportTypes.h"
#include "OmnibusPlayData.generated.h"

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

	EOmniPlayMode GetPlayMode() const { return PlayMode; }
	void          SetPlayMode(EOmniPlayMode InPlayMode) { PlayMode = InPlayMode; }

	UStaticMesh*  GetPlainRoadMesh(const int& InMeshIdx) const;
	UStaticMesh*  GetIntersectionFlat4WayRoadMesh(const int& InMeshIdx) const;

	UTexture2D*   GetCityBlockIcon(ECityBlockType InCityBlockType) const;

	TSubclassOf<AOmniLineBusRoute> GetOmniLineBusRouteClass() const { return OmniLineBusRouteClass; }
	TSubclassOf<AOmniVehicleBus> GetOmniVehicleBusClass() const { return OmniVehicleBusClass; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AOmniLineBusRoute> OmniLineBusRouteClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Class", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AOmniVehicleBus> OmniVehicleBusClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road", meta = (AllowPrivateAccess = "true"))
	TArray<FRoadStaticMeshSet> RoadMeshSets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CityBlock", meta = (AllowPrivateAccess = "true"))
	TArray<FCityBlockIcon> CityBlockIcons;
	
	/** 현재 플레이 중인 모드. 편집, 이동 등. */
	EOmniPlayMode PlayMode;
	FVector3d CurrentLocation;
};
