// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActorFactory.h"
#include "EnochGameModeBase.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "FLAnimLoader.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnochGameInstance.h"

AEnochActorFactory* AEnochActorFactory::Factory;
bool AEnochActorFactory::FactoryReady = false;

// Sets default values
AEnochActorFactory::AEnochActorFactory()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	
	//melee
	loadFreelancerTemplateClass(1, TEXT("Blueprint'/Game/DataForCli/Blueprint/Melee1.Melee1_C'"));
	loadFreelancerTemplateClass(2, TEXT("Blueprint'/Game/DataForCli/Blueprint/Melee1.Melee1_C'"));
	loadFreelancerTemplateClass(3, TEXT("Blueprint'/Game/DataForCli/Blueprint/Melee1.Melee1_C'"));
	loadFreelancerTemplateClass(4, TEXT("Blueprint'/Game/DataForCli/Blueprint/Melee1.Melee1_C'"));
	loadFreelancerTemplateClass(5, TEXT("Blueprint'/Game/DataForCli/Blueprint/Melee1.Melee1_C'"));

	//ranged
	loadFreelancerTemplateClass(6, TEXT("Blueprint'/Game/DataForCli/Blueprint/Ranged1.Ranged1_C'"));
	loadFreelancerTemplateClass(7, TEXT("Blueprint'/Game/DataForCli/Blueprint/Ranged1.Ranged1_C'"));
	loadFreelancerTemplateClass(8, TEXT("Blueprint'/Game/DataForCli/Blueprint/Ranged1.Ranged1_C'"));
	loadFreelancerTemplateClass(9, TEXT("Blueprint'/Game/DataForCli/Blueprint/Ranged1.Ranged1_C'"));
	loadFreelancerTemplateClass(10, TEXT("Blueprint'/Game/DataForCli/Blueprint/Ranged1.Ranged1_C'"));

	//assassin
	loadFreelancerTemplateClass(11, TEXT("Blueprint'/Game/DataForCli/Blueprint/Assassin1.Assassin1_C'"));
	loadFreelancerTemplateClass(12, TEXT("Blueprint'/Game/DataForCli/Blueprint/Assassin1.Assassin1_C'"));
	loadFreelancerTemplateClass(13, TEXT("Blueprint'/Game/DataForCli/Blueprint/Assassin1.Assassin1_C'"));
	loadFreelancerTemplateClass(14, TEXT("Blueprint'/Game/DataForCli/Blueprint/Assassin1.Assassin1_C'"));
	loadFreelancerTemplateClass(15, TEXT("Blueprint'/Game/DataForCli/Blueprint/Assassin1.Assassin1_C'"));

	//special
	loadFreelancerTemplateClass(16, TEXT("Blueprint'/Game/DataForCli/Blueprint/Special1.Special1_C'"));
	loadFreelancerTemplateClass(17, TEXT("Blueprint'/Game/DataForCli/Blueprint/Special1.Special1_C'"));
	loadFreelancerTemplateClass(18, TEXT("Blueprint'/Game/DataForCli/Blueprint/Special1.Special1_C'"));
	loadFreelancerTemplateClass(19, TEXT("Blueprint'/Game/DataForCli/Blueprint/Special1.Special1_C'"));
	loadFreelancerTemplateClass(20, TEXT("Blueprint'/Game/DataForCli/Blueprint/Special1.Special1_C'"));

	//crawler
	loadFreelancerTemplateClass(21, TEXT("Blueprint'/Game/DataForCli/Blueprint/Crawler.Crawler_C'"));

	//range attack
	loadProjectileTemplateClass(-1, TEXT("Blueprint'/Game/DataForCli/Blueprint/Projectile.Projectile_C'"));

	//skill
	loadProjectileTemplateClass(30000, TEXT("Blueprint'/Game/DataForCli/Blueprint/Projectile.Projectile_C'"));
	loadProjectileTemplateClass(30001, TEXT("Blueprint'/Game/DataForCli/Blueprint/YellowArc.YellowArc_C'"));
	loadProjectileTemplateClass(30002, TEXT("Blueprint'/Game/DataForCli/Blueprint/Stomp.Stomp_C'"));
}
void AEnochActorFactory::loadFreelancerTemplateClass(int TID, const TCHAR * path)
{
	ConstructorHelpers::FClassFinder<AEnochFreeLancer> loadedClass(path);
	if (loadedClass.Succeeded())
		FreelancerTemplateClass.Add(TID, loadedClass.Class);
	else
		UE_LOG(Enoch, Warning, TEXT("tid %d load failed = path : %s"), TID, path);
}
void AEnochActorFactory::loadProjectileTemplateClass(int TID, const TCHAR* path)
{
	ConstructorHelpers::FClassFinder<AEnochProjectile> loadedClass(path);
	if (loadedClass.Succeeded())
		ProjectileTemplateClass.Add(TID, loadedClass.Class);
	else
		UE_LOG(Enoch, Warning, TEXT("tid %d load failed = path : %s"), TID, path);
}
// Called when the game starts or when spawned
void AEnochActorFactory::BeginPlay()
{
	Super::BeginPlay();
	CommonFactory = MakeUnique<EnochActorDataFactory>();
	CommonFactory->BeginPlay();

	auto world = GetWorld();
	if (!world) return;

	Factory = this;
	int32 SerialNumber = 0;
}

void AEnochActorFactory::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Factory = nullptr;
	//이 함수 호출시 모든 Actor들의 data 포인터가 할당이 해제되므로, 그전에 data포인터를 참조하는 모든 로직을 종료해야함
	CommonFactory->EndPlay();
	CommonFactory = nullptr;
}

FLBattleData* AEnochActorFactory::GetFreeLancerData(int32 SerialNumber) 
{
	if (EnochActorDataFactory::instance == nullptr)
		return nullptr;
	else
		return EnochActorDataFactory::instance->GetFreeLancerData(SerialNumber); 
}

EnochProjectileData* AEnochActorFactory::GetProjectileData(int32 SerialNumber)
{ 
	if (EnochActorDataFactory::instance == nullptr)
		return nullptr;
	else
		return EnochActorDataFactory::instance->GetProjectileData(SerialNumber); 
}

//액터를 오브젝트 풀에서 꺼냄
AEnochFreeLancer* AEnochActorFactory::SpawnFreeLancerByTID(uint8 type, uint8 level, bool isEnemy)
{
	if (!GetFactoryReady()) return nullptr;
	auto SerialNumber = Factory->CommonFactory->SpawnFreeLancer();
	if (SerialNumber < 0) return nullptr;
	auto world = Factory->GetWorld();
	if (world == nullptr) return nullptr;
	auto freelancer = world->SpawnActor<AEnochFreeLancer>(Factory->FreelancerTemplateClass[type]);
	Factory->FreeLancerMap.Add(SerialNumber);
	Factory->FreeLancerMap[SerialNumber] = freelancer;

	freelancer->SetActive(true);
	freelancer->SerialNumber = SerialNumber;
	freelancer->GetData()->InitStatus(type, level, SerialNumber, isEnemy);
	freelancer->InitFL(SerialNumber);
	return freelancer;
}
AEnochFreeLancer* AEnochActorFactory::SpawnFreeLancerBySN(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;
	if (SerialNumber < 0) return nullptr;
	auto world = Factory->GetWorld();
	if (world == nullptr) return nullptr;
	auto rawdata = Factory->CommonFactory->GetFreeLancerData(SerialNumber);
	if (rawdata == nullptr) return nullptr;
	auto freelancer = world->SpawnActor<AEnochFreeLancer>(Factory->FreelancerTemplateClass[rawdata->GetTID()]);
	Factory->FreeLancerMap.Add(SerialNumber);
	Factory->FreeLancerMap[SerialNumber] = freelancer;

	freelancer->SetActive(true);
	freelancer->SerialNumber = SerialNumber;
	freelancer->GetData()->InitStatus(rawdata->GetTID(), rawdata->GetLevel(), SerialNumber, rawdata->isEnemy);
	freelancer->InitFL(SerialNumber);
	return freelancer;
}

AEnochProjectile* AEnochActorFactory::SpawnProjectile(int32 SerialNumber)
{
	if (!GetFactoryReady()) return nullptr;
	auto rawdata = Factory->CommonFactory->GetProjectileData(SerialNumber);
	if (rawdata == nullptr) return nullptr;
	auto world = Factory->GetWorld();
	if (world == nullptr) return nullptr;
	
	auto location = AEnochField::GetLocation(rawdata->location.x, rawdata->location.y);
	auto transform = FTransform(location);
	auto projectile = world->SpawnActor<AEnochProjectile>(Factory->ProjectileTemplateClass[rawdata->templateID], transform);
	projectile->SetActive(false);
	if (rawdata->templateID == -1) //default attack
	{
		auto owner = GetFreeLancer(rawdata->_owner);
		if (owner != nullptr)
			projectile->Flipbook->SetProjectileCopy(owner->Flipbook);
	}
	
	Factory->ProjectileMap.Add(SerialNumber);
	Factory->ProjectileMap[SerialNumber] = projectile;

	projectile->SerialNumber = SerialNumber;

	auto radius = atan2(rawdata->direction.x, -rawdata->direction.y) / PI * 180;
	auto rotation = FRotator(180, radius, -90);//atan2(rawdata->direction.y, rawdata->direction.x));
	projectile->Flipbook->SetWorldRotation(rotation);
	auto owner = GetFreeLancer(rawdata->_owner);
	if (owner != nullptr && rawdata->templateID < 0)
		projectile->Flipbook->SetRelativeLocation(owner->Flipbook->GetRelativeLocation());
	projectile->Tick(0);
	projectile->SetActive(true);
	return projectile;
}

//사용이 끝난 프리랜서 제거
void AEnochActorFactory::Release(AEnochFreeLancer* actor, bool update)
{
	if (Factory == nullptr) return;

	//액터가 필드에 붙어있는 경우, 필드에서 분리
	auto Cell = AEnochField::GetCell(actor->GetFieldLocation());
	if (Cell)
		Cell->DetachFreeLancer();
	
	//액터를 제거
	actor->SetActive(false);
	auto data = actor->GetData();
	if (data != nullptr) {
		Factory->FreeLancerMap.Remove(actor->SerialNumber);	//액터 제거
		Factory->CommonFactory->DeleteFreeLancer(actor->SerialNumber);	//데이터 제거
	}
	actor->Destroy();
	// 동맹 업데이트
	if(update)
		AEnochActorFactory::UpdateAlliance();
}

void AEnochActorFactory::Release(AEnochProjectile* actor)
{
	if (Factory == nullptr) return;
	if (actor == nullptr) return;

	actor->SetActive(false);
	actor->Destroy();
	Factory->ProjectileMap.Remove(actor->SerialNumber);	//remove actor
}

AEnochFreeLancer* AEnochActorFactory::GetFreeLancer(int32 SerialNumber)
{
	if (Factory == nullptr) return nullptr;
	auto actor = Factory->FreeLancerMap.Find(SerialNumber);
	if (actor == nullptr)
		return nullptr;
	else
		return *actor;
}

AEnochProjectile* AEnochActorFactory::GetProjectile(int32 SerialNumber)
{
	if (Factory == nullptr) return nullptr;
	auto actor = Factory->ProjectileMap.Find(SerialNumber);
	
	if (actor == nullptr)
		return nullptr;
	else
		return *actor;
}

bool AEnochActorFactory::GetFactoryReady()
{
	return Factory != nullptr &&
			Factory->CommonFactory.IsValid();
}

void AEnochActorFactory::ClearProjectiles()
{
	if (Factory == nullptr) return;
	for (auto& pair : Factory->ProjectileMap) {
		pair.Value->SetActive(false);
		pair.Value->Destroy();
	}
	Factory->ProjectileMap = TMap<int, class AEnochProjectile*>();
}

//Alliance 관련 버프 정보 업데이트
void AEnochActorFactory::UpdateAlliance()
{
	auto& factory = Factory->CommonFactory;
	factory->UpdateAllianceAll();
	factory->UpdateFLAll();	//현재 빈 함수
	// UI 업데이트
	auto gi = Cast<UEnochGameInstance>(Factory->GetGameInstance());
	if(gi && gi->MyPlayMenuUI)
		gi->MyPlayMenuUI->UpdateAllianceText(factory->strAlly, factory->strEnemy);
}