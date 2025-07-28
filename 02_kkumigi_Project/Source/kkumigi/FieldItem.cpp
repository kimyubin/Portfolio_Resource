// Fill out your copyright notice in the Description page of Project Settings.


#include "FieldItem.h"

#include "FieldItemFactory.h"
#include "ItemManager.h"
#include "KuGameInstance.h"
#include "KuTypes.h"
#include "Components/BoxComponent.h"

#include "UTLStatics.h"

const FPrimaryAssetType AFieldItem::ItemPrimaryAssetType = TEXT("FieldItemActor"); 

AFieldItem::AFieldItem()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

	CursorDetector = CreateDefaultSubobject<UBoxComponent>(TEXT("CursorDetector"));
	CursorDetector->SetupAttachment(RootComponent);
	CursorDetector->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	CursorDetector->SetBoxExtent(FVector(50.f));
	CursorDetector->SetRelativeScale3D(FVector(2.f));
	CursorDetector->SetGenerateOverlapEvents(true);
	CursorDetector->SetCollisionProfileName(FKuCollisionProfile::UITraceOnly);

	bIsRotating        = false;
	TargetRelativeQuat = FQuat();
}

FPrimaryAssetId AFieldItem::GetPrimaryAssetId() const
{
	FString AssetName = GetName();
	AssetName.RemoveFromStart(DEFAULT_OBJECT_PREFIX);
	AssetName.RemoveFromStart(TEXT("REINST_"));
	AssetName.RemoveFromStart(TEXT("SKEL_"));
	AssetName.RemoveFromStart(TEXT("GC_"));
	AssetName.RemoveFromEnd(TEXT("_C"));

	return FPrimaryAssetId(AFieldItem::ItemPrimaryAssetType, FName(AssetName));
}

void AFieldItem::BeginPlay()
{
	Super::BeginPlay();

	TargetRelativeQuat = RootComponent->GetRelativeRotation().Quaternion();
}

void AFieldItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetItemManager()->GetFieldItemFactory()->RemoveFieldItem(this);

	Super::EndPlay(EndPlayReason);
}

void AFieldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateAnimItem(DeltaTime);
}

void AFieldItem::SetPreviewMode()
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents(UPrimitiveComponent::StaticClass(), Components, true);

	for (UPrimitiveComponent* Child : Components)
	{
		Child->SetCollisionProfileName(FKuCollisionProfile::PreviewFieldItem);
	}
	CursorDetector->SetCollisionProfileName(FKuCollisionProfile::UITraceOnly);
}

bool AFieldItem::SetPreviewToRealMode()
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents(UPrimitiveComponent::StaticClass(), Components, true);

	for (UPrimitiveComponent* Child : Components)
	{
		Child->SetCollisionProfileName(FKuCollisionProfile::FieldItem);
	}
	CursorDetector->SetCollisionProfileName(FKuCollisionProfile::UITraceOnly);
	return true;
}

void AFieldItem::TrackCursorForPreview(const FHitResult& InHitResult, AActor* InHitActor)
{
	const FQuat ActorQuat       = GetActorQuat();
	const FVector ActorUpVector = ActorQuat.GetUpVector();
	const FVector ImpactNormal  = InHitResult.ImpactNormal;

	// 액터회전 -> 히트표면노말에 대한 수직축(회전축)
	const FVector RotationAxis = FVector::CrossProduct(ActorUpVector, ImpactNormal).GetSafeNormal();

	// 액터회전과 히트표면노말 사이의 끼인각
	const float DotProduct    = FVector::DotProduct(ActorUpVector, ImpactNormal);
	const float RotationAngle = acosf(DotProduct);

	// 현재 회전에 DeltaQuat만큼 회전 추가
	const FQuat DeltaQuat = FQuat(RotationAxis, RotationAngle);
	const FQuat NewQuat   = DeltaQuat * ActorQuat;


	SetActorLocationAndRotation(InHitResult.Location, NewQuat);
}

void AFieldItem::RotateAnimItem(const float InDeltaTime)
{
	if (bIsRotating == false)
	{
		return;
	}

	const float TurningSpeed = GetItemManager()->GetFieldItemFactory()->GetItemRotateSpeed();

	FQuat NewRelativeQuat = FMath::QInterpTo(RootComponent->GetRelativeRotation().Quaternion(), TargetRelativeQuat, InDeltaTime, TurningSpeed);

	// 오차 범위 내면, 목표 각도를 바로 적용함.
	if (TargetRelativeQuat.Equals(NewRelativeQuat, 0.001))
	{
		NewRelativeQuat = TargetRelativeQuat;
		bIsRotating     = false;
	}

	SetActorRelativeRotation(NewRelativeQuat);
}

void AFieldItem::RotateItem(const double InDegree)
{
	if (bIsRotating == false)
	{
		TargetRelativeQuat = RootComponent->GetRelativeRotation().Quaternion();
	}
	bIsRotating = true;

	const FQuat NewRelRotQuat = TargetRelativeQuat * FRotator(0.0, InDegree, 0.0).Quaternion();
	TargetRelativeQuat        = NewRelRotQuat;
}

void AFieldItem::RotateItemByStep(const float InInputStep)
{
	const double Step = UtlMath::GetSign(InInputStep);
	RotateItem(90.0 * Step);
}

void AFieldItem::SetCustomDepthStencilAndPreviewMode(const EStencilOutline StencilColor, const bool IsPreviewMod)
{
	
	bool bHoverUI = false;
	UMaterial* BuildableMat = nullptr;
	
	if (IsPreviewMod)
	{
		/*
		if(StencilColor == EStencilOutline::Preview_Buildable)
			BuildableMat = GetGameInstance<UKuGameInstance>()->GetCitadelPlayData()->GetBuildableMaterial();
		else if(StencilColor == EStencilOutline::Preview_Unbuildable)
			BuildableMat = GetGameInstance<UKuGameInstance>()->GetCitadelPlayData()->GetUnbuildableMaterial();
		*/	
		// bHoverUI = GetWorld()->GetFirstPlayerController<ACitadelPlayerController>()->IsHoverUI();		
	}
	for (const TWeakObjectPtr<UMeshComponent>& ChildWeak : MeshComponents)
	{
		if (UMeshComponent* Child = ChildWeak.Get())
		{
			Child->SetCustomDepthStencilValue(UtlEnum::EnumToInt(StencilColor));
			
			//미리보기 모드일 때, 모든 하위 메시를 미리보기용 투명 머티리얼로 교체
			if (IsPreviewMod)
			{
				// 커서가 UI위에 있으면, 외곽선도 완전히 숨기기위해 visibility도 false 적용
				if (bHoverUI)
					Child->SetVisibility(false);

				const int MaterialNum = Child->GetNumMaterials();
				for (int i = 0; i < MaterialNum; ++i)
				{
					Child->SetMaterial(i, BuildableMat);
				}
			}			
		}
	}
}

void AFieldItem::SetBuildingVisibility(bool bVisibility)
{
	// UI위에서 숨기기
	for (const TWeakObjectPtr<UMeshComponent>& ChildWeak : MeshComponents)
	{
		if (UMeshComponent* Child = ChildWeak.Get())
			Child->SetVisibility(bVisibility);
	}
}

void AFieldItem::CollectMeshChildren()
{
	TArray<USceneComponent*> ChildrenList;
	RootComponent->GetChildrenComponents(true, ChildrenList);
	MeshComponents.Empty();

	for (USceneComponent* Child : ChildrenList)
	{
		if (Child->IsA(UMeshComponent::StaticClass()))
			MeshComponents.Add(Cast<UMeshComponent>(Child));		
	}
}

