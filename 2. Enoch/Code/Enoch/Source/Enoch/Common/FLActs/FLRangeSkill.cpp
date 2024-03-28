#include "FLRangeSkill.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochProjectileData.h"
#include "../EnochSimulator.h"
#include "../PJActs/PJGuidedArrow.h"
#include "../PJActs/PJLinearArrow.h"


void FLRangeSkill::_Begin()
{
	auto targetData = data->GetTarget();
	if (targetData == nullptr) return;
	target = targetData->GetSerialNumber();
	data->direction = targetData->locationOnFight - data->locationOnFight;
	skillData = *SkillData::GetSkillTemplate(data->skill);
	data->mpNow = 0;
	//EnochSimulator::logs.push({ SimulateInfoType::Attack, data->GetSerialNumber(), targetData->GetSerialNumber()});
}

void FLRangeSkill::_End()
{
	auto damage = 0;
	for (auto e : skillData.levelData[data->GetLevel()].effects)
		if (e.first == SkillEffect::PUREDMG)
			damage = e.second;
	if (skillData.target == SkillTarget::Enemy_Around)
		target = data->GetSerialNumber();
	auto targetData = EnochActorDataFactory::instance->GetFreeLancerData(target);
	if (targetData == nullptr) return;
	data->direction = targetData->locationOnFight - data->locationOnFight;
	auto factory = EnochActorDataFactory::instance;
	auto SN_PJ = factory->SpawnProjectile();
	//if (SN_PJ < 0) 오류상황 처리안됨
	auto projectileData = factory->GetProjectileData(SN_PJ);
		
	if (projectileData != nullptr) {
		auto simulator = EnochSimulator::instance;
		simulator->ProjectileQueue->push_back(SN_PJ);
		projectileData->Set(damage, 20.0f, targetData->GetSerialNumber(), data->GetSerialNumber(),&skillData);
		EnochSimulator::logs.push({ SimulateInfoType::Skill, data->GetSerialNumber(), targetData->GetSerialNumber(),SN_PJ});
	}
	data->SetState(FreeLancerState::PostSkill);
	time = 0;
}