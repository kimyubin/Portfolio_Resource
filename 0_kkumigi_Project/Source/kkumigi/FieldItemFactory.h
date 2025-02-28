// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuActor.h"
#include "KuTypes.h"
#include "GameFramework/Actor.h"
#include "FieldItemFactory.generated.h"

class AKuPlayerController;
class AFieldItem;

UCLASS()
class KKUMIGI_API AFieldItemFactory : public AKuActor
{
	GENERATED_BODY()

public:
	AFieldItemFactory();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	/**
	 * 미리보기 아이템을 스폰합니다.
	 * AFieldItem UClass 데이터를 비동기 Load하여 스폰합니다.
	 * 
	 * @param InPreviewItemID 스폰 대상 ItemID 
	 */
	void SpawnAsyncPreviewItem(const TItemDefID InPreviewItemID);


	/**
	 * Load 완료된 아이템을 스폰합니다.
	 * 
	 * @param InItemID 
	 * @param InSpawnTransform 
	 * @return 
	 */
	AFieldItem* GetSpawnItemOnPostLoad(const TItemDefID InItemID, const FTransform& InSpawnTransform);

	/**
	 * 비동기 Load 완료 후 아이템을 스폰하는 델리게이트용 함수입니다.
	 * 
	 * @param InPreviewItemID 
	 * @param InSpawnTransform 
	 */
	void OnPostLoadSpawnItem(const TItemDefID InPreviewItemID, const FTransform InSpawnTransform);

	/**
	 * 비동기 Load 이후 실행할 델리게이트용 함수입니다.
	 * Load가 완료되면 발동합니다.
	 * 
	 * @param InPreviewItemID 스폰 대상 ItemID
	 * @param InSpawnTransform 스폰 트랜스폼
	 */
	void OnPostLoadSpawnPreviewItem(const TItemDefID InPreviewItemID, const FTransform InSpawnTransform);

	void AddFieldItem(AFieldItem* InFieldItem);
	void RemoveFieldItem(AFieldItem* InFieldItem);

	/**
	 * 미리보기 아이템 커서 추적
	 * 커서를 추적하는 델리게이트 바인드용 함수입니다.
	 */
	void OnTrackCursorWithPreview(AKuPlayerController* InKuPC);

	/** 임시 아이템을 정식 스폰 */
	void SetItemToReal();

	void CancelPreviewItem();

	bool IsItemPreviewMode() const { return bIsPreviewMode; }

	/***/
	void RotateItem(const float InInputValue);

	/** 임시 아이템이 스폰되어져있고, 커서가 UI 위에 있을 때 */
	void SetHoverUI(const bool InbHover);

	float GetItemRotateSpeed() const { return ItemRotateSpeed; };

protected:
	/***/
	bool bIsPreviewMode;

	/** 커서가 UI이 위에 있는지를 확인합니다. 위에 있을 때 클릭 및 마우스 오버를 금지하기 위함. */
	bool bHoverUI;

	/** 미리보기용 임시 아이템 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AFieldItem> PreviewFieldItem;

	/** 선택한 아이템 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<AFieldItem> SelectedFieldItem;

	/** 필드에 깔린 아이템 목록 */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TSet<TWeakObjectPtr<AFieldItem>> FieldItemList;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float ItemRotateSpeed;
};
