#include "FLMeleeAttack.h"
#include "../EnochSimulator.h"
#include "../EnochActorDataFactory.h"
#include "../EnochFreeLancerData.h"

void FLMeleeAttack::_Begin()
{
	auto targetData = data->GetTarget();
	if (targetData == nullptr) return;
	target = targetData->GetSerialNumber();
	data->direction = targetData->locationOnFight - data->locationOnFight;
	EnochSimulator::logs.push({ SimulateInfoType::Attack, data->GetSerialNumber(), targetData->GetSerialNumber() });
}

void FLMeleeAttack::_End()
{
	auto targetData = EnochActorDataFactory::instance->GetFreeLancerData(target);
	if(targetData != nullptr)// && time >= length && data->CanAttack())
		FLBattleData::DoAttack(data, targetData);
	data->SetState(FreeLancerState::PostAttack);
	//EnochSimulator::logs.push({ SimulateInfoType::Attack, data->GetSerialNumber(), target->GetSerialNumber(),-1 });
	time = 0;
}

