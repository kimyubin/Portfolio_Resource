// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelForeverTypes.h"
#include "GameFramework/Actor.h"
#include "CitadelBuilding.generated.h"

UCLASS()
class CITADELFOREVER_API ACitadelBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	ACitadelBuilding();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StartCursorOver(UPrimitiveComponent* Component);

	UFUNCTION(BlueprintCallable)
	void EndCursorOver(UPrimitiveComponent* Component);

	UFUNCTION(BlueprintCallable)
	void ClickAndDoubleClick(UPrimitiveComponent* Component, FKey InputKey);

	/** 임시 -> 일반 모드로 변경 후 건설 모드*/
	UFUNCTION(BlueprintCallable)
	void StartBuild();

	/** 임시 빌딩 모드로 변경. 모든 자식 컴포넌트 머티리얼을 변경*/
	UFUNCTION(BlueprintCallable)
	void RunPreviewMode();
	
	/** 임시 빌딩 모드일 때 커서 따라다님. */
	UFUNCTION(BlueprintCallable)
	virtual void TrackCursorForPreview(const FVector& InLocation, AActor* InHitActor);

	/** 임시 빌딩 모드 일 때, 건설 불가 지역에서 색 바꿔줌 */
	UFUNCTION()
	void EnterUnBuildableArea(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/** 임시 빌딩 모드 일 때, 건설 불가 지역에서 색 바꿔줌 */
	UFUNCTION()
	void LeaveUnBuildableArea(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/**
	 * 스텐실 적용
	 * @param StencilColor stencil value, 'enum class EStencilOutline' 사용
	 */
	UFUNCTION(BlueprintCallable)
	void SetCustomDepthStencil(EStencilOutline StencilColor);

	/** 설치 가불에 대한 상태 변경*/
	UFUNCTION(BlueprintCallable)
	void ChangeInstallableState(bool bInInstallable);

	/** 빌딩 선택 모드로 전환 */
	UFUNCTION(BlueprintCallable)
	void SetSelectedBuildingMode(bool InSelect);
	
	/**
	 * 스텐실 적용
	 * @param StencilColor stencil value, 'enum class EStencilOutline' 사용
	 * @param IsPreviewMod 미리보기일 경우 투명화 처리
	 */
	UFUNCTION(BlueprintCallable)
	void SetCustomDepthStencilAndPreviewMode(const EStencilOutline StencilColor, const bool IsPreviewMod = false);
	
	/** UI 위에서 임시 빌딩 숨기기*/
	UFUNCTION(BlueprintCallable)
	void SetBuildingVisibility(bool bVisibility);

	/** 자식 메시 컴포넌트 수집*/
	void CollectMeshChildren();	
	
	
	//마우스 감지 및 충돌, 겹침 감지 컴포넌트
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BuildingCollisionBox;
	
	/** 저장 등 사용되는 건물 기초 정보. 타입, 위치, 방향 레벨*/
	FBuildingInfo MyBuildingInfo;

	TArray<TWeakObjectPtr<UMeshComponent>> MeshComponentChildren;

	/** 용병 이름 및 HP 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* UnifiedUpperUI;
	*/

	/** 미리보기 모드인지 확인*/
	bool bPreviewMode;
	/** 성벽 위에 있는 지 확인*/
	bool bOnRampart;
	/** 다른 빌딩과 겹치는 지 확인*/
	bool bOverlapOtherBuilding;
	/** 선택 중인지 확인*/
	bool bSelectedMode;
	
	/** 직전 클릭 시간 저장*/
	double PreviousClickTime;

	/** 더블클릭 간격 조정*/
	UPROPERTY(EditAnywhere)
	float DoubleClickInterval;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 AttackPower;

	/** 공격 범위*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	double AttackRange;

	/** 공격 속도. 높을 수록 빠르게 공격함. 공격 딜레이는 공속의 역수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 AttackSpeed;

	/** 방어력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Status)
	int32 Defense;
public:
	/** 공격 범위에 실제 언리얼 단위 거리를 곱해서 반환*/
	double GetAttackRange() const { return AttackRange * CitadelConstant::Unit_Range; }
	bool CanInstall() { return (bOnRampart || !bOverlapOtherBuilding); }
	bool IsOnRampart() { return bOnRampart; }
	
};
