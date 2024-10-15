// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActor.h"
#include "PlayDefaultPawn.h"
#include "EnochGameModeBase.h"

AEnochActor::AEnochActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RENDER"));
	AnimGroupComponent = CreateDefaultSubobject<USceneComponent>(TEXT("AnimGroup"));
	AnimGroupComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Flipbook = CreateDefaultSubobject<UEnochPaperFlipbookComponent>(TEXT("FLIPBOOK"));
	Flipbook->AttachToComponent(AnimGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AEnochActor::BeginPlay()
{
	Super::BeginPlay();
	RotatorSetforCamera();
}

void AEnochActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotatorSetforCamera();
}

void AEnochActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnochActor::SetActive(bool active)
{
	SetActorHiddenInGame(!active);	//화면 렌더 여부
	if (!active) SetActorTransform(FTransform(FVector(-1000.0f, 0.0f, 0.0f))); //게임에 영향을 못끼치게 멀리 떨어뜨림
	SetActorTickEnabled(active);	//틱 호출 통제
}

void AEnochActor::RotatorSetforCamera()
{
	//액터 전체 이미지 기울기 결정.
	//기본 pawn에 달린 카메라 기울기로 결정함.
	//카메라 암 y축(pitch) 기울기의 수직 성분만 따옴.
	APlayDefaultPawn* EDefaultPawn = Cast<APlayDefaultPawn>(Cast<AEnochGameModeBase>(GetWorld()->GetAuthGameMode())->DefaultPawnClass.GetDefaultObject());
	if (EDefaultPawn != nullptr)
	{
		AnimGroupComponent->SetRelativeRotation(APlayDefaultPawn::GetCameraArmRotate());
	}
}