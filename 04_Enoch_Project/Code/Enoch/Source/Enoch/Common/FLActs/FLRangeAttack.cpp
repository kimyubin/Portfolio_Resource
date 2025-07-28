#include "FLRangeAttack.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochProjectileData.h"
#include "../EnochSimulator.h"
#include "../PJActs/PJGuidedArrow.h"

void FLRangeAttack::_Begin()
{
	auto targetData = data->GetTarget();
	if (targetData == nullptr) return;
	target = targetData->GetSerialNumber();
	data->direction = targetData->locationOnFight - data->locationOnFight;
	EnochSimulator::logs.push({ SimulateInfoType::Attack, data->GetSerialNumber(), targetData->GetSerialNumber()});
	//EnochSimulator::logs.push({ SimulateInfoType::Attack, data->GetSerialNumber(), targetData->GetSerialNumber()});
}

void FLRangeAttack::_End()
{
	auto targetData = EnochActorDataFactory::instance->GetFreeLancerData(target);
	if (targetData == nullptr) return;
	auto factory = EnochActorDataFactory::instance;
	auto SN_PJ = factory->SpawnProjectile();
	//if (SN_PJ < 0) 오류상황 처리안됨
	auto projectileData = factory->GetProjectileData(SN_PJ);
	if (projectileData != nullptr) {
		auto simulator = EnochSimulator::instance;
		simulator->ProjectileQueue->push_back(SN_PJ);
		projectileData->Set(data->attackDamage, 5.0f, targetData->GetSerialNumber(), data->GetSerialNumber());
		EnochSimulator::logs.push({ SimulateInfoType::SpawnPJ, SN_PJ });
	}
	data->SetState(FreeLancerState::PostAttack);
	time = 0;
	
}