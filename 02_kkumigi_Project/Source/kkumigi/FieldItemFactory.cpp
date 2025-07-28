// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldItemFactory.h"

#include "FieldItem.h"
#include "KuAssetManager.h"
#include "KuGameInstance.h"
#include "KuPlayData.h"
#include "KuPlayerController.h"

#include "UTLStatics.h"

// Sets default values
AFieldItemFactory::AFieldItemFactory()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsPreviewMode = false;
	bHoverUI      = false;

	ItemRotateSpeed = 15.0f;
}

// Called when the game starts or when spawned
void AFieldItemFactory::BeginPlay()
{
	Super::BeginPlay();

	// 아이템 미리보기 트랙킹 델리게이트 바인딩
	if (AKuPlayerController* KuPlayerPC = Cast<AKuPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		KuPlayerPC->GetOnTrackCursorDelegate().BindUObject(this, &AFieldItemFactory::OnTrackCursorWithPreview);		
	}

}

// Called every frame
void AFieldItemFactory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFieldItemFactory::SpawnAsyncPreviewItem(const TItemDefID InPreviewItemID)
{
	if (InPreviewItemID == INDEX_NONE)
		return;

	// 이미 미리보기 아이템이 있다면 제거.
	if (bIsPreviewMode)
		CancelPreviewItem();

	bIsPreviewMode = true;

	AKuPlayerController* KuPlayerPC = Cast<AKuPlayerController>(GetWorld()->GetFirstPlayerController());
	if (KuPlayerPC == nullptr)
		return;

	FHitResult Result;
	KuPlayerPC->GetHitResultUnderCursor(ECC_Visibility, false, Result);
	FTransform SpawnTransform = FTransform{FRotator::ZeroRotator, Result.Location};

	// 데이터 비동기 로드
	UKuAssetManager& KuAssetManager = UKuAssetManager::Get();
	FStreamableDelegate OnPostLoad  = FStreamableDelegate::CreateUObject(this, &AFieldItemFactory::OnPostLoadSpawnPreviewItem, InPreviewItemID, SpawnTransform);
	KuAssetManager.LoadFieldItem(InPreviewItemID, OnPostLoad);
}

AFieldItem* AFieldItemFactory::GetSpawnItemOnPostLoad(const TItemDefID InItemID, const FTransform& InSpawnTransform)
{
	UKuAssetManager& KuAssetManager = UKuAssetManager::Get();
	TSubclassOf<AFieldItem> FieldItemClass = KuAssetManager.GetFieldItemClass(InItemID);

	return GetWorld()->SpawnActor<AFieldItem>(FieldItemClass, InSpawnTransform);
}

void AFieldItemFactory::OnPostLoadSpawnItem(const TItemDefID InPreviewItemID, const FTransform InSpawnTransform)
{
	
}

void AFieldItemFactory::OnPostLoadSpawnPreviewItem(const TItemDefID InPreviewItemID, const FTransform InSpawnTransform)
{
	PreviewFieldItem = GetSpawnItemOnPostLoad(InPreviewItemID, InSpawnTransform);

	if (PreviewFieldItem.IsValid())
	{
		PreviewFieldItem->SetPreviewMode();
	}
}

void AFieldItemFactory::AddFieldItem(AFieldItem* InFieldItem)
{
	FieldItemList.Emplace(InFieldItem);
}

void AFieldItemFactory::RemoveFieldItem(AFieldItem* InFieldItem)
{
	FieldItemList.Remove(TWeakObjectPtr<AFieldItem>(InFieldItem));
}

void AFieldItemFactory::OnTrackCursorWithPreview(AKuPlayerController* InKuPC)
{
	if (IsValid(InKuPC) == false)
	{
		return;
	}
	if (bHoverUI)
	{
		return;
	}

	if (bIsPreviewMode && PreviewFieldItem.IsValid())
	{
		TArray<FHitResult> HitResults;
		InKuPC->GetMultiHitResultsUnderCursor(HitResults, FKuCollisionChannel::FieldItemTrace);

		for (FHitResult& HitResult : HitResults)
		{
			if (AActor* HitActor = HitResult.GetActor())
			{
				if (PreviewFieldItem == Cast<AFieldItem>(HitActor))
					continue;

				PreviewFieldItem->TrackCursorForPreview(HitResult, HitActor);
				break;
			}
		}
	}
}

void AFieldItemFactory::SetItemToReal()
{
	if (bHoverUI)
	{
		return;
	}
	if (bIsPreviewMode == false)
	{
		return;
	}

	if (AFieldItem* PreviewItemPtr = PreviewFieldItem.Get())
	{
		if (PreviewItemPtr->SetPreviewToRealMode())
		{
			AddFieldItem(PreviewItemPtr);
			PreviewFieldItem.Reset();
			bIsPreviewMode = false;
		}
	}
}

void AFieldItemFactory::CancelPreviewItem()
{
	bIsPreviewMode = false;
	if (PreviewFieldItem.IsValid())
	{
		PreviewFieldItem->Destroy();
		PreviewFieldItem.Reset();
	}
}

void AFieldItemFactory::RotateItem(const float InInputValue)
{
	if (InInputValue == 0.0)
	{
		return;
	}

	if (bIsPreviewMode && PreviewFieldItem.IsValid())
	{
		PreviewFieldItem->RotateItemByStep(InInputValue);
	}
	else if (SelectedFieldItem.IsValid())
	{
		SelectedFieldItem->RotateItemByStep(InInputValue);
	}
}

void AFieldItemFactory::SetHoverUI(const bool InbHover)
{
	if (bHoverUI == InbHover)
	{
		return;
	}
	if (bIsPreviewMode == false)
	{
		return;
	}
}
