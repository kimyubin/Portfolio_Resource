// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniActor.h"
#include "OmnibusTypes.h"
#include "PathVisualizator.generated.h"

struct FTransferPath;
class USplineComponent;
class USplineMeshComponent;
/**
 * 
 */
UCLASS()
class OMNIBUS_API APathVisualizator : public AOmniActor
{
public:
	APathVisualizator();

private:
	GENERATED_BODY()

public:
	/** 스플라인 컴포넌트의 마지막 PointIdx 반환. */
	int32 GetLastPointIdx() const;

	void MakeSplineMesh(const FVector& InStartLoc, const FVector& InStartTangent, const FVector& InEndLoc, const FVector& InEndTangent);

	/**환승경로 시각화하기*/
	void VisualizeTransferRoutes(const FTransferPath& InTransferPath, const FLinearColor& InPathColor = FLinearColor::Black, const FVector& InPathOffset = FVector(0.0, 0.0, 30.0));

	void SetPathVisibility(const bool SetVisibility) const;

	bool IsSamePath(const FTransferPath& InOtherPath) const;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	USplineComponent* NewSpline;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FLinearColor PathColor;

	/** 경로 시각화의 오프셋. 높이를 조절함. */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector PathOffset;

	/** 캐싱을 위해 각자의 경로를 갖고 있음. */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	FTransferPath MyTransferPath;		
};
