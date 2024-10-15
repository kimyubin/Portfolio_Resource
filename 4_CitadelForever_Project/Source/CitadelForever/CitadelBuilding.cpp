// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CitadelBuilding.h"

#include "CitadelBuildingFactory.h"
#include "CitadelDefaultPawn.h"
#include "CitadelForever.h"
#include "CitadelForeverGameInstance.h"
#include "CitadelGameUIsHandler.h"
#include "CitadelPlayData.h"
#include "CitadelPlayerController.h"
#include "CitadelRampart.h"
#include "CitadelStatics.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"


ACitadelBuilding::ACitadelBuilding()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	
	BuildingCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BuildingCollisionBox"));	
	BuildingCollisionBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	BuildingCollisionBox->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	BuildingCollisionBox->SetBoxExtent(FVector(50.f));
	BuildingCollisionBox->SetRelativeScale3D(FVector(2.f));
	BuildingCollisionBox->SetGenerateOverlapEvents(true);
	BuildingCollisionBox->SetCollisionProfileName(TEXT("UI"));
	
	Tags.Add(ECitadelTag::CitadelBuilding);
	BuildingCollisionBox->ComponentTags.Add(ECitadelTag::CitadelBuilding);

	bPreviewMode = false;
	bOnRampart = false;
	bOverlapOtherBuilding = false;
	bSelectedMode = false;
	
	PreviousClickTime = 0.;
	DoubleClickInterval = 0.5f;

	/*
	UnifiedUpperUI = CreateDefaultSubobject<UWidgetComponent>(TEXT("UnifiedUI"));
	
	static ConstructorHelpers::FClassFinder<UUserWidget> UnifiedUpperUIClass(TEXT("/Game/UI/FreeLancerUnifiedUpperUIBP"));
	
	if (UnifiedUpperUIClass.Succeeded())
	{
		UnifiedUpperUI->SetWidgetClass(UnifiedUpperUIClass.Class);
		UnifiedUpperUI->SetDrawSize(FVector2D(100,50));
		UnifiedUpperUI->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);
		UnifiedUpperUI->SetRelativeLocation(FVector(0.f,0.f,125.f));
		UnifiedUpperUI->SetWidgetSpace(EWidgetSpace::Screen);
	}
	*/
}

void ACitadelBuilding::BeginPlay()
{
	Super::BeginPlay();

	//빌딩 수집
	ACitadelBuildingFactory* CitadelBuildingFactory = GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelBuildingFactory();
	if (IsValid(CitadelBuildingFactory))
		CitadelBuildingFactory->AddBuilding(this);

	CollectMeshChildren();	
	for (const auto& Child : MeshComponentChildren)
	{
		if (Child.IsValid())		
			Child->SetRenderCustomDepth(true);		
	}
	
	//마우스 오버, 클릭, 더블클릭 이벤트 바인딩
	BuildingCollisionBox->OnBeginCursorOver.AddDynamic(this, &ACitadelBuilding::StartCursorOver);
	BuildingCollisionBox->OnEndCursorOver.AddDynamic(this, &ACitadelBuilding::EndCursorOver);
	BuildingCollisionBox->OnClicked.AddDynamic(this, &ACitadelBuilding::ClickAndDoubleClick);

}
void ACitadelBuilding::StartCursorOver(UPrimitiveComponent* Component)
{
	//프리뷰 빌딩이 필드 내에 존재하면 사용안함.
	const bool HasPreviewBuilding = GetWorld()->GetFirstPlayerController<ACitadelPlayerController>()->HasPreview();

	if (!bPreviewMode && !bSelectedMode && !HasPreviewBuilding)	
		SetCustomDepthStencil(EStencilOutline::Friendly_Hover);
}

void ACitadelBuilding::EndCursorOver(UPrimitiveComponent* Component)
{
	const bool HasPreviewBuilding = GetWorld()->GetFirstPlayerController<ACitadelPlayerController>()->HasPreview();
	
	if (!bPreviewMode && !bSelectedMode && !HasPreviewBuilding)	
		SetCustomDepthStencil(EStencilOutline::None);
}

void ACitadelBuilding::ClickAndDoubleClick(UPrimitiveComponent* Component, FKey InputKey)
{
	const bool HasPreviewBuilding = GetWorld()->GetFirstPlayerController<ACitadelPlayerController>()->HasPreview();

	if (InputKey == EKeys::LeftMouseButton && !bPreviewMode && !HasPreviewBuilding)
	{
		const double CurrentTime = GetWorld()->GetRealTimeSeconds();
		auto CFGInstance = GetGameInstance<UCitadelForeverGameInstance>();

		if(CurrentTime - PreviousClickTime < DoubleClickInterval)
		{
			// only doubleClick logic
			CFGInstance->GetCitadelDefaultPawn()->GoToActor(GetActorLocation());
		}
		
		//클릭, 더블클릭 공통 로직
		CFGInstance->GetCitadelGameUIsHandler()->SelectBuilding(this);		
		
		PreviousClickTime = CurrentTime;		
	}
}

void ACitadelBuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ACitadelBuildingFactory* CitadelBuildingFactory = GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelBuildingFactory();
	if (IsValid(CitadelBuildingFactory))
		CitadelBuildingFactory->RemoveBuilding(this);

	Super::EndPlay(EndPlayReason);
}

void ACitadelBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACitadelBuilding::StartBuild()
{
	//todo: 건설 애니메이션 로직 작성
	GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelGameUIsHandler()->SelectBuilding(this);
}

void ACitadelBuilding::TrackCursorForPreview(const FVector& InLocation, AActor* InHitActor)
{
}

void ACitadelBuilding::RunPreviewMode()
{
	bPreviewMode = true;
	//건설 불가지역 진입 이벤트 바인딩
	BuildingCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACitadelBuilding::EnterUnBuildableArea);
	BuildingCollisionBox->OnComponentEndOverlap.AddDynamic(this,&ACitadelBuilding::LeaveUnBuildableArea);
	
	// 외곽선 그리기 / 미리보기 메시 교체 / UI 위에서 숨김
	SetCustomDepthStencilAndPreviewMode(EStencilOutline::Preview_Buildable, true);
}

void ACitadelBuilding::EnterUnBuildableArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->ActorHasTag(ECitadelTag::CitadelBuilding))
	{
		bOverlapOtherBuilding = true;
	}
}

void ACitadelBuilding::LeaveUnBuildableArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{	
	if(OtherActor->ActorHasTag(ECitadelTag::CitadelBuilding))
	{
		bOverlapOtherBuilding = false;
	}
}

void ACitadelBuilding::SetCustomDepthStencil(EStencilOutline StencilColor)
{
	SetCustomDepthStencilAndPreviewMode(StencilColor, false);
}
void ACitadelBuilding::ChangeInstallableState(bool bInInstallable)
{
	if(bInInstallable)
		SetCustomDepthStencilAndPreviewMode(EStencilOutline::Preview_Buildable, true);
	else
		SetCustomDepthStencilAndPreviewMode(EStencilOutline::Preview_Unbuildable, true);
}

void ACitadelBuilding::SetSelectedBuildingMode(bool InSelect)
{
	bSelectedMode = InSelect;
	if(InSelect)
	{		
		SetCustomDepthStencil(EStencilOutline::Friendly_Select);
	}
	else
	{		
		SetCustomDepthStencil(EStencilOutline::None);
	}
	
}

void ACitadelBuilding::SetCustomDepthStencilAndPreviewMode(const EStencilOutline StencilColor, const bool IsPreviewMod)
{
	bool bHoverUI = false;
	UMaterial* BuildableMat = nullptr;
	
	if (IsPreviewMod)
	{
		if(StencilColor == EStencilOutline::Preview_Buildable)
			BuildableMat = GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData()->GetBuildableMaterial();
		else if(StencilColor == EStencilOutline::Preview_Unbuildable)
			BuildableMat = GetGameInstance<UCitadelForeverGameInstance>()->GetCitadelPlayData()->GetUnbuildableMaterial();
			
		bHoverUI = GetWorld()->GetFirstPlayerController<ACitadelPlayerController>()->IsHoverUI();		
	}
	for (const auto& Child : MeshComponentChildren)
	{
		if (Child.IsValid())
		{
			Child->SetCustomDepthStencilValue(EnumToInt(StencilColor));
			
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

void ACitadelBuilding::SetBuildingVisibility(bool bVisibility)
{
	// UI위에서 숨기기
	for (const auto& Child : MeshComponentChildren)
	{
		if (Child.IsValid())
			Child->SetVisibility(bVisibility);		
	}
}

void ACitadelBuilding::CollectMeshChildren()
{
	TArray<USceneComponent*> ChildrenList;
	RootComponent->GetChildrenComponents(true, ChildrenList);
	MeshComponentChildren.Empty();
	
	for (const auto& Child : ChildrenList)
	{
		if (Child->IsA(UMeshComponent::StaticClass()))
			MeshComponentChildren.Add(Cast<UMeshComponent>(Child));		
	}
}