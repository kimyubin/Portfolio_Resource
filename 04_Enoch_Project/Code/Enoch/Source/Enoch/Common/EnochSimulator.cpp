// Fill out your copyright notice in the Description page of Project Settings.

#include "EnochSimulator.h"
#include "EnochFiniteStateMachine.h"
#include "FLActs/EnochFreeLancerAct.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldData.h"
#include "EnochFreeLancerData.h"
#include "EnochFieldCellData.h"
#include "EnochProjectileData.h"
#include "../Enoch.h"
#include <memory>

queue<SimulateInfo> EnochSimulator::logs;
float EnochSimulator::simulateTime;
EnochSimulator* EnochSimulator::instance;

EnochSimulator::EnochSimulator()
{
	simulateTime = 0;
	result = GameResult::None;
	simulating = false;
	simulateEndTime = 0;
	numEnemyFreeLancer = 0;
	numPlayerFreeLancer = 0;
	logs = std::queue<SimulateInfo>();
	instance = this;
}

EnochSimulator::~EnochSimulator()
{
	logs = std::queue<SimulateInfo>();
	
}

void EnochSimulator::BeginPlay()
{
	//FreeLancer 큐 초기화. 맵 위의 모든 용병이 큐에 들어감
	//FreeLancerQueue = make_unique<PriorityQueue<FLBattleData>>();
	EnochActorDataFactory::instance->UpdateAllianceAll();
	FreeLancerQueue = make_unique<vector<int>>();
	for(int i=0; i<EnochFieldData::GetHeight(); i++) {
		for (int j=0;j<EnochFieldData::GetWidth();j++) {
			auto cell = EnochFieldData::GetData({i,j});
			auto freeLancer = EnochActorDataFactory::instance->GetFreeLancerData(cell->FreeLancerOn);
			if (freeLancer != nullptr) {
				UE_LOG(Enoch, Warning, TEXT("%d : %d"), freeLancer->GetTID(),freeLancer->state);
				freeLancer->BeginSimulate();
				cell->FreeLancerOnFight = freeLancer->GetSerialNumber();
				FreeLancerQueue->push_back(freeLancer->GetSerialNumber());
				if (freeLancer->isEnemy) numEnemyFreeLancer++;
				else numPlayerFreeLancer++;
			}
		}
	}
	/*
	//aliiance 카운트
	for (auto SerialNumber : *FreeLancerQueue)
	{
		auto freeLancer = EnochActorDataFactory::instance->GetFreeLancerData(SerialNumber);
		if(!freeLancer->isEnemy)
	}*/
	ENLOG(Warning, TEXT("%d %d"), numEnemyFreeLancer, numPlayerFreeLancer);
	//액터가 없는상태로 시작한 경우의 예외 처리
	//Projectile 큐 초기화
	ProjectileQueue = make_unique<vector<int>>();
	//ProjectileQueue = new PriorityQueue<EnochProjectileData>();
	if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
		result = GameResult::Draw;
	else if (numEnemyFreeLancer == 0)
		result = GameResult::Win;
	else if (numPlayerFreeLancer == 0)
		result = GameResult::Lose;
	else {
		result = GameResult::UnderSimulated;
		simulating = true;
		simulateEndTime = FLT_MAX;
	}
	if(!simulating)
		EnochSimulator::logs.push({ SimulateInfoType::EndSimulator });
}

void EnochSimulator::EndPlay()
{
	//Act클래스 정리
	//FreeLancerAct::Release();

	//맵 위의 모든 용병 분리
	for (int i = 0; i < EnochFieldData::GetHeight(); i++) {
		for (int j = 0; j < EnochFieldData::GetWidth(); j++) {
			auto cell = EnochFieldData::GetData({ i,j });
			if (!cell) continue;
			cell->FreeLancerOnFight = -1;
		}
	}
	if (EnochActorDataFactory::instance != nullptr) {
		for (auto& index : *FreeLancerQueue)
		{
			auto freelancer = EnochActorDataFactory::instance->GetFreeLancerData(index);
			if (freelancer == nullptr) continue;
			freelancer->SetState(FreeLancerState::Idle);
			freelancer->nextAttackTime = 0;
		}
	}
	ProjectileQueue->clear();
	FreeLancerQueue->clear();
}

void EnochSimulator::BeginTick(float DeltaTime)
{
	//시뮬레이터의 시간은 Tick이 호출될때만 흐름
}

shared_ptr<pair<vector<int>, vector<int>>> EnochSimulator::Tick(float DeltaTime)
{
	simulateTime += DeltaTime;
	auto results = make_shared<pair<vector<int>, vector<int>>>();

	//Remove Expired Projectiles
	for (auto it = ProjectileQueue->begin(); it != ProjectileQueue->end();) {
		auto projectile = EnochActorDataFactory::instance->GetProjectileData(*it);
		if (projectile == nullptr)
			ProjectileQueue->erase(it);
		else if (projectile->expired) {
			EnochSimulator::logs.push({ SimulateInfoType::RemovePJ, projectile->SerialNumber });
			ProjectileQueue->erase(it);
		}
		else it++;
	}

	//시뮬레이션 종료 후 일정시간이 지나 승패판정
	if (!simulating && simulateTime > simulateEndTime) {
		if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
			result = GameResult::Draw;
		else if (numEnemyFreeLancer == 0)
			result = GameResult::Win;
		else if (numPlayerFreeLancer == 0)
			result = GameResult::Lose;
		EnochSimulator::logs.push({ SimulateInfoType::EndSimulator });
	}
	numEnemyFreeLancer = 0;
	numPlayerFreeLancer = 0;
	//Tick FreeLancers
	for (auto& SN_FL : *FreeLancerQueue)
		FreeLancerTick(EnochActorDataFactory::instance->GetFreeLancerData(SN_FL), DeltaTime);

	//Tick Projectiles
	for (auto& SN_PJ : *ProjectileQueue)
		ProjectileTick(EnochActorDataFactory::instance->GetProjectileData(SN_PJ), DeltaTime);

	if (numEnemyFreeLancer == 0 || numPlayerFreeLancer == 0) {
		if (numEnemyFreeLancer == 0 && numPlayerFreeLancer == 0)
			result = GameResult::Draw;
		else if (numEnemyFreeLancer == 0)
			result = GameResult::Win;
		else if (numPlayerFreeLancer == 0)
			result = GameResult::Lose;
		simulating = false;
		EnochSimulator::logs.push({ SimulateInfoType::EndSimulator });
	}

	//return unused
	return results;
}

void EnochSimulator::FreeLancerTick(FLBattleData *freeLancer, float deltaTime)
{
	//ignore Invalids
	if(freeLancer == nullptr) return;
	
	if(freeLancer->GetState() == FreeLancerState::Dead) return;
	
	if (freeLancer->isEnemy) numEnemyFreeLancer++;
	else numPlayerFreeLancer++;

	//check if Game is Over
	if (!simulating && freeLancer->GetState() != FreeLancerState::Victory) {
		freeLancer->SetState(FreeLancerState::Victory);
		return;
	}
	//modify state
	freeLancer->Act(deltaTime);
}

void EnochSimulator::ProjectileTick(EnochProjectileData* projectile, float deltaTime)
{
	if (projectile == nullptr) {
		EnochSimulator::logs.push({ SimulateInfoType::Debug, 8 });
		return;
	}

	if (simulating)
		projectile->Act(deltaTime);
}
