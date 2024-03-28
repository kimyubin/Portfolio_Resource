// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFight.h"
#include "EnochActorFactory.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "EnochGameModeBasePlay.h"
#include "EnochGameInstance.h"

bool AEnochFight::simulated = false;
// Sets default values
AEnochFight::AEnochFight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnochFight::BeginPlay()
{
	Super::BeginPlay();
	delay = 0;
	speed = 100;
	if (simulated) {
		ENLOG(Warning, TEXT("AEnochFight Should Be Destroied Before Recreate it."));
		Destroy();
		return;
	}
	Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI->Switcher_Buttons->SetActiveWidgetIndex(1);
	Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI->InitBattleSpeed();
	simulated = true;
	//이함수 호출 전에 원래 상대팀이 추가되는 이펙트와 함께, 상대유닛들이 상대진영에 소환되어야함.
	StartFight();
}

void AEnochFight::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Cast<UEnochGameInstance>(GetGameInstance())->MyPlayMenuUI->Switcher_Buttons->SetActiveWidgetIndex(0);
	Simulator.EndPlay();
	simulated = false;
}

void AEnochFight::StartFight()
{
	Simulator.BeginPlay();
}

//승리화면 이후 실행되는 함수
void AEnochFight::EndFight()
{
	//캐릭터 전투 이전 위치로 리셋
	AEnochField::forAllAttachedFreeLancer([](AEnochFreeLancer* freelancer, FIntPoint pos) {
			auto zero = AEnochField::GetCell(FIntPoint(0,0));
			auto nextZero = AEnochField::GetCell(FIntPoint(1,1));
			auto d = nextZero->GetActorLocation() - zero->GetActorLocation();
			auto convertedLoc = zero->GetActorLocation() + FVector(d.X * pos.X, d.Y * pos.Y, 0);
			auto result = freelancer->SetActorLocation(convertedLoc);
			freelancer->Reset();
		});
	//시뮬레이터 결과 보고 및 화면 송출
	Cast<AEnochGameModeBasePlay>(GetWorld()->GetAuthGameMode())->SetFightResult(Simulator.result);
	Simulator.EndPlay();
	Destroy();
}

// 제한시간을 두려면, 여기에서 DeltaTime의 합을 계산해서 제한시간이 넘자마자 Tick을 멈추면 될듯
// Called every frame
void AEnochFight::Tick(float DeltaTime)
{
	delay += speed;
	Super::Tick(DeltaTime);

	if (!simulated) return;
	//이번틱에 움직인 모든 결과를 받음
	while(delay >= 100) {
		delay -= 100;
		Simulator.Tick(DeltaTime);
		processLog();
	}
}

void AEnochFight::processLog()
{
	while (!EnochSimulator::logs.empty())
	{
		auto& log = EnochSimulator::logs.front();
		switch (log.type)
		{//추후 제대로된 로그 클래스를 구현?
			//로그타입에 따라 필요한 변수의 개수와 형태가 다름..
			//뚱뚱한 로그클래스 vs 상속 후 다이나믹캐스트 vs 상속 후 key/value로 변수 리턴
		case SimulateInfoType::Attack: {
			auto freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
			if (freelancer != nullptr)
				freelancer->Attack();
			if (log.val3 >= 0)
				auto pj = AEnochActorFactory::SpawnProjectile(log.val3);
			break;
		}
		case SimulateInfoType::Debug:
			//ENLOG(Warning, TEXT("Debug Log, %d, %d, %d"), log.val1, log.val2, log.val3);
			break;
		case SimulateInfoType::Move: {
			auto freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
			//freelancer->SetActorLocation(AEnochField::GetCell(FVector2D(log.val2, log.val3))->GetActorLocation());
			//ENLOG(Warning, TEXT("%d번 프리랜서가 이동했다!"), log.val1);
			freelancer->Move();
			break;
		}
		case SimulateInfoType::CriticalDamage :
			ENLOG(Warning, TEXT("%d번 프리랜서가 %d의 크리티컬 데미지를 주었다! "), log.val1, log.val3);
		case SimulateInfoType::Damage: {
			AEnochActorFactory::GetProjectile(log.val1);
			auto freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
			//if (freelancer != nullptr)
			//	freelancer->ActInit();
			break;
		}
		case SimulateInfoType::RemovePJ: {
			auto projectile = AEnochActorFactory::GetProjectile(log.val1);
			//UE_LOG(Enoch, Warning, TEXT("1"));
			if(projectile != nullptr)
				projectile->Bomb();
			break;
		}
		case SimulateInfoType::SpawnPJ: {
			if (log.val1 >= 0) {
				auto pj = AEnochActorFactory::SpawnProjectile(log.val1);
			}
			break;
		}
		case SimulateInfoType::Skill : {
			//ENLOG(Warning, TEXT("%d번 프리랜서가 %d번 프리랜서에게 스킬 원거리 공격을 시도했다! 생성된 투사체 %d"), log.val1, log.val2, log.val3);
			auto freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
			if (freelancer != nullptr)
				freelancer->Skill();
			if (log.val3 >= 0) {
				auto pj = AEnochActorFactory::SpawnProjectile(log.val3);
			}
			break;
		}
		case SimulateInfoType::EndSimulator: {
			//2초 후 필드 정리 및 결과화면 예약
			GetWorldTimerManager().SetTimer(resultTimer, this, &AEnochFight::EndFight, 2.0f, false);
			
			//승리포즈
			AEnochField::forAllAttachedFreeLancer([](AEnochFreeLancer* freelancer, FIntPoint pos) {
				if (freelancer->GetState() != FreeLancerState::Dead) {
					freelancer->Victory();
				}
				});
			AEnochActorFactory::ClearProjectiles();
			simulated = false;
			return;
		}
		case SimulateInfoType::Buff: {
			auto freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
			if (freelancer != nullptr)
			{
				if(log.val2 == (int)SkillEffect::STUN)
					freelancer->Idle();
			}
			break;
		}
		case SimulateInfoType::Dead: {
			auto freelancer = AEnochActorFactory::GetFreeLancer(log.val1);
			freelancer->Dead();
			break;
		}
		}
		EnochSimulator::logs.pop();
	}
}