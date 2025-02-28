// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KuTypes.h"
#include "Engine/AssetManager.h"
#include "KuAssetManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class KKUMIGI_API UKuAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Returns the AssetManager singleton object.
	static UKuAssetManager& Get();

	void Init();

	/** id로 정렬한 ItemDataTable 배열을 반환합니다. */
	const TArray<const FItemTableRow*>& GetSortedTableRows();

	void LoadFieldItem(const TItemDefID InItemID, FStreamableDelegate DelegateToCall);
	void LoadItemUIImage(const TItemDefID InItemID, FStreamableDelegate DelegateToCall);
	FPrimaryAssetId GetFieldItemPrimaryAssetId(const TItemDefID InItemID) const;

	/**
	 * 로드가 완료된 FieldItem class를 가져옵니다.
	 * 로드 요청 완료 이후 호출되어야 합니다.
	 */
	TSubclassOf<AFieldItem> GetFieldItemClass(const TItemDefID InItemID) const;

	/**
	 * 로드가 완료된 UI용 이미지 Object를 가져옵니다.
	 * 로드 요청 완료 이후 호출되어야 합니다.
	 */
	UTexture2D* GetItemUIImage(const TItemDefID InItemID) const;

protected:
	/** 아이템 관련 정보와 리소스 Path를 저장합니다. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ItemDataTable;

	/** ItemDataTable을 id로 정렬한 배열을 캐싱합니다. */
	TArray<const FItemTableRow*> CachedSortedTableRows;

};
