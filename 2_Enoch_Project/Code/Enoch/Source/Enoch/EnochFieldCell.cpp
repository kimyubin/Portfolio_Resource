// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFieldCell.h"
#include "EnochFreeLancer.h"
#include "EnochActorFactory.h"
#include "Components/StaticMeshComponent.h"

UMaterial* AEnochFieldCell::WhiteMaterial;
UMaterial* AEnochFieldCell::BlackMaterial;

// Sets default values
AEnochFieldCell::AEnochFieldCell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//감지용 태그
	Tags.Add(FName(TEXT("EnochFieldCell")));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PLANE(\
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	static bool valid = InitMaterial();
	
	if (PLANE.Succeeded()) {
		Mesh->SetStaticMesh(PLANE.Object);
	}
}

//체스판의 머터리얼 두가지(흑,백)을 불러와서 전역변수에 저장해두는 함수
bool AEnochFieldCell::InitMaterial()
{
	ConstructorHelpers::FObjectFinder<UMaterial> WHITE_MATERIAL(\
		TEXT("/Game/DataForCli/FloorMaterial1.FloorMaterial1"));
	ConstructorHelpers::FObjectFinder<UMaterial> BLACK_MATERIAL(\
		TEXT("/Game/DataForCli/FloorMaterial2.FloorMaterial2"));

	if (!WHITE_MATERIAL.Succeeded() ||
		!BLACK_MATERIAL.Succeeded()) 
		return false;
	WhiteMaterial = WHITE_MATERIAL.Object;
	BlackMaterial = BLACK_MATERIAL.Object;

	return true;
}

// Called when the game starts or when spawned
void AEnochFieldCell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnochFieldCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//필드 셀의 위치를 입력된 xy값에 맞게 옮기고, 위치에 맞는 색상 부여함.(바둑판식)
void AEnochFieldCell::SetFieldLocation(int32 y, int32 x)
{
	this->SetActorRelativeLocation(FVector(-450 + 100 * x, -450+100*y, 20));
	if ((x + y) % 2 == 0)
		Mesh->SetMaterial(0, BlackMaterial);
	else
		Mesh->SetMaterial(0, WhiteMaterial);
}

void AEnochFieldCell::AttachFreeLancer(AEnochFreeLancer* arg)
{ 
	auto FreeLancerData = arg->GetData();
	if (FreeLancerData == nullptr) return;
	DetachFreeLancer();
	arg->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	data->SetFreeLancerOn(FreeLancerData);
	arg->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	arg->GetData()->location = data->GetLocation();
	AttachedFreeLancer = arg->GetData()->GetSerialNumber();
}

void AEnochFieldCell::DetachFreeLancer()
{
	auto freelancer = GetAttachedFreeLancer();
	if(freelancer != nullptr)
		freelancer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	AttachedFreeLancer = -1;
}

class AEnochFreeLancer* AEnochFieldCell::GetAttachedFreeLancer()
{
	return AEnochActorFactory::GetFreeLancer(AttachedFreeLancer);
}

bool AEnochFieldCell::HasAttachFreeLancer()
{ 
	return GetAttachedFreeLancer() != nullptr;
}