// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuActor.h"
#include "GameFramework/Actor.h"

#include "FieldItem.generated.h"


enum class EStencilOutline : uint8;
class UBoxComponent;

/**
 * 필드에 배치할 수 있는 아이템
 */
UCLASS()
class KKUMIGI_API AFieldItem : public AKuActor
{
	GENERATED_BODY()

public:
	AFieldItem();

	/** 에셋의 프라이머리 에셋 아이디를 지정합니다. */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	

public:
	virtual void Tick(float DeltaTime) override;

	void SetPreviewMode();

	/**
	 * 미리보기 모드에서 실제 모드로 변환
	 * @return 설치 불가 지역이면 false 리턴
	 */
	bool SetPreviewToRealMode();

	/**
	 * 매 틱마다 커서를 따라갑니다. (미리보기 모드 한정)
	 * 
	 * @param InHitResult 커서가 가리키는 위치에 대한 정보
	 * @param InHitActor 커서가 가리키는 액터 정보
	 */
	void TrackCursorForPreview(const FHitResult& InHitResult, AActor* InHitActor);

	/** 아이템 회전 애니메이션 로직을 수행하는 함수. */
	void RotateAnimItem(const float InDeltaTime);
	
	/**
	 * 아이템을 상대 z축으로 회전시킵니다.
	 * 
	 * @param InDegree 회전시킬 각도. 
	 */
	void RotateItem(const double InDegree);

	/**
	 * 90도 간격으로 회전합니다.
	 * 
	 * @param InInputStep 크기에 상관없이 음수, 양수인지만 구분합니다.
	 */
	void RotateItemByStep(const float InInputStep);

protected:
	/** 커서 감지 컴포넌트. 기본 단순, 복합 콜리전보다 러프하게 선택하기 위한 UI용 콜리전. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CursorDetector;

	/** 아이템 중심 위치 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USceneComponent* CenterPoint;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))

	/** 회전 애니메이션 도중이면 true */
	bool bIsRotating;

	/** 회전 목표 */
	FQuat TargetRelativeQuat;

	/** 자식 메시 컴포넌트. 커스템 뎁스 컨트롤을 위해 리스트에 담아놓았습니다. */
	TArray<TWeakObjectPtr<UMeshComponent>> MeshComponents;

	void SetCustomDepthStencilAndPreviewMode(const EStencilOutline StencilColor, const bool IsPreviewMod);

	void SetBuildingVisibility(bool bVisibility);
	void CollectMeshChildren();

public:
	/** 필드 아이템의 프라이머리 에셋 타입 */
	static const FPrimaryAssetType ItemPrimaryAssetType;
};
