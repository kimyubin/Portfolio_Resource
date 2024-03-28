// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochField.h"

#include "EnochActorFactory.h"
#include "EnochFieldCell.h"
#include "EnochFreeLancer.h"
#include "EnochGameInstance.h"
#include "EnochGameModeBase.h"
#include "Components/BoxComponent.h"

AEnochField* AEnochField::Cell;
// Sets default values
AEnochField::AEnochField()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));

	RootComponent = Mesh;
}

// Called when the game starts or when spawned
void AEnochField::BeginPlay()
{
	Super::BeginPlay();

	// 필드 초기화(10x10개의 셀 생성 및 배치)
	auto world = GetWorld();
	if (!world) return;

	Field = MakeUnique<TArray<TArray<AEnochFieldCell*>>>();

	// 실제 데이터도 초기화
	data.InitData(EnochFieldData::GetHeight(), EnochFieldData::GetWidth());

	// TArray 안에 액터들 초기화
	(*Field.Get()).Reserve(EnochFieldData::GetHeight());
	for (int i = 0; i < EnochFieldData::GetHeight(); i++)
	{
		(*Field.Get()).Add(TArray<AEnochFieldCell*>());
		(*Field.Get())[i].Reserve(EnochFieldData::GetWidth());
		for (int j = 0; j < EnochFieldData::GetWidth(); j++)
		{
			auto actor = world->SpawnActor<AEnochFieldCell>();
			if (!actor) continue;
			actor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			actor->SetFieldLocation(i, j);
			(*Field.Get())[i].Add(actor);
			(*Field.Get())[i][j]->data = data.GetData({i, j});
		}
	}
	Cell = this;

	//이거 있으면 Factory beginplay가 작동함. if문 없이 그냥 호출만 해도 작동함. 
	if(AEnochActorFactory::GetFactoryReady())
		GenerateField();
	//게임 인스턴스에 접근할 수 있게 변경.
	Cast<UEnochGameInstance>(GetGameInstance())->MyEnochField = this;
}

void AEnochField::GenerateField()
{
	UEnochGameInstance* EGameInstance = Cast<UEnochGameInstance>(GetGameInstance());
	for (int i = 0; i < EnochFieldData::GetHeight(); i++)
	{
		for (int j = 0; j < EnochFieldData::GetWidth(); j++)
		{
			//프리랜서 제거
			auto AttachedFL = (*Cell->Field)[i][j]->GetAttachedFreeLancer();			
			if(AttachedFL)
				AttachedFL->Release(false);
			//프리랜서 생성
			auto FLTID = EGameInstance->commander->GetFieldFreeLancerList(j,i).FLTemplateID;
			auto level = EGameInstance->commander->GetFieldFreeLancerList(j,i).FLLevel;
			if(FLTID==FreeLancerTemplateID::None)
				continue;
			bool isEnemy = i >= 5;
			auto FreeLancer = AEnochActorFactory::SpawnFreeLancerByTID(FLTID, level, isEnemy);
			if(!FreeLancer)
			{
				UE_LOG(LogTemp, Display, TEXT("Display Log Message"));
				continue;
			}
			//액터를 클릭한 필드에 붙이기
			(*Field.Get())[i][j]->AttachFreeLancer(FreeLancer);
		}
	}
	// 동맹 업데이트
	AEnochActorFactory::UpdateAlliance();
}

void AEnochField::forAllAttachedFreeLancer(std::function<void(AEnochFreeLancer* freelancer, FIntPoint)> func)
{
	for (auto y = 0; GetCell(FIntPoint(0,y)) != nullptr; y++) {
		for (auto x = 0; GetCell(FIntPoint(x,y)) != nullptr; x++) {
			auto freelancer = GetCell(FIntPoint(x,y))->GetAttachedFreeLancer();
			if (freelancer != nullptr)
				func(freelancer, FIntPoint(x,y));
		}
	}
}


void AEnochField::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Common 클래스는 가비지컬렉션이 작동하지않으므로, 스스로 메모리를 정리할 수 있도록 유도
	Cell = nullptr;
	data.EndPlay();
}

// Called every frame
void AEnochField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AEnochField::GetLocation(float x, float y)
{
	auto zero = AEnochField::GetCell(FIntPoint(0, 0));
	auto nextZero = AEnochField::GetCell(FIntPoint(1, 1));
	auto d = nextZero->GetActorLocation() - zero->GetActorLocation();
	auto convertedLoc = zero->GetActorLocation() + FVector(d.X * x, d.Y * y, 10);
	return convertedLoc;
}