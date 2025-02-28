// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ItemPrimaryDataAsset.generated.h"

class AFieldItem;

/**
 * 사용하지 않음.
 * 아이템에 관련된 에셋과 경로를 모아놓는 데이터 에셋.
 */
UCLASS()
class KKUMIGI_API UItemPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", AssetBundles = "ItemActor"))
	TSoftClassPtr<AFieldItem> FieldItem;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", AssetBundles = "ItemActor, UI"))
	TSoftObjectPtr<UTexture2D> ItemImg;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true", AssetBundles = "ItemActor, UI, Tag", Categories="Item"))
	FGameplayTagContainer ItemTag;
};
