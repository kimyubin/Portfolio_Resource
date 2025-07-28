// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoxelFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UVoxelFunctionLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * 구의 영역 안에 있는 복셀들의 월드 좌표를 계산합니다.
	 * 
	 * @param InCenterPos 구의 중심 좌표 
	 * @param InRadius 구의 반경
	 * @param InVoxelLength 각 복셀의 단위 길이. 복셀의 간격으로 사용됩니다.
	 * @return 복셀의 월드 좌표 배열
	 */
	static TArray<FVector> SphereVoxelWorldPositions(const FVector& InCenterPos, const float InRadius, const float InVoxelLength);

	/**
	 * 중심좌표와 한변의 길이로 정의된,
	 * Box 영역 안에 있는 복셀들의 월드 좌표를 계산합니다.
	 * 
	 * @param InCenterPos Box의 중심 좌표
	 * @param InLength Box 한 변의 길이
	 * @param InVoxelLength 각 복셀의 단위 길이. 복셀의 간격으로 사용됩니다.
	 * @return 복셀의 월드 좌표 배열 
	 */
	static TArray<FVector> BoxVoxelWorldPositionsByLength(const FVector& InCenterPos, const float InLength, const float InVoxelLength);

	/**
	 * 중심좌표와 각 모서리의 길이로 정의된,
	 * Box 영역 안에 있는 복셀들의 월드 좌표를 계산합니다.
	 * 
	 * @param InCenterPos Box의 중심 좌표
	 * @param InExtent Box의 각 모서리 길이 
	 * @param InVoxelLength 각 복셀의 단위 길이. 복셀의 간격으로 사용됩니다. 
	 * @return 복셀의 월드 좌표 배열 
	 */
	static TArray<FVector> BoxVoxelWorldPositionsByExtent(const FVector& InCenterPos, const FVector& InExtent, const float InVoxelLength);

	/**
	 * 두 점을 대각으로하는 
	 * Box 영역 안에 있는 복셀들의 월드 좌표를 계산합니다.
	 * 
	 * @param InPointA 
	 * @param InPointB 
	 * @param InVoxelLength 각 복셀의 단위 길이. 복셀의 간격으로 사용됩니다. 
	 * @return 복셀의 월드 좌표 배열 
	 */
	static TArray<FVector> BoxVoxelWorldPositions(const FVector& InPointA, const FVector& InPointB, const float InVoxelLength);
};
