// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelForeverTypes.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/PlayerController.h"
#include "CitadelPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CITADELFOREVER_API ACitadelPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	/** 임시 빌딩 스폰 및 추적*/
	UFUNCTION(BlueprintCallable)
	void SpawnAndTrackPreviewBuilding(EBuildingType PreviewType);
	
	/** 임시 빌딩 건설모드로 변경*/
	UFUNCTION(BlueprintCallable)
	void SpawnBuilding();

	UFUNCTION(BlueprintCallable)
	void CancelBuild();
	
	/** 커서 아래로 멀티 라인 트래이스. 아래에 있는 모든 객체를 채널 기반으로 체크 */
	UFUNCTION(BlueprintCallable)
	void GetMultiHitResultsUnderCursor(TArray<FHitResult>& InHitResults, ECollisionChannel TraceChannel);

	/** 입력 맵핑에 대한 환경 정보. 문맥 정보 안에는 InputActionsConfig에 담긴 InputAction이 있고, 각 InputAction의 맵핑이 담겨 있음. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input")
	class UCitadelInputConfig* InputActionsConfig;
	
protected:
	
	bool IsPreviewMode;
	bool bHoverUI;			//UI이 위에 있을 때 클릭 및 마우스 오버 금지용.
	TWeakObjectPtr<class ACitadelBuilding> PC_Weak_PreviewBuilding;

public:
	void HoverUI();
	void UnHoverUI();
	bool HasPreview() { return IsPreviewMode; }
	bool IsHoverUI() { return bHoverUI; }
};
