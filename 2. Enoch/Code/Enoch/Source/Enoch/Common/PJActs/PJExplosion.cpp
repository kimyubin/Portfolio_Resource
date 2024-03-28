#include "PJExplosion.h"
#include "../EnochProjectileData.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochSimulator.h"
#include "../../Enoch.h"
#include "../SkillData.h"

void PJExplosion::_Begin()
{
	length = 10;
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);
	if (ownerPtr == nullptr) 
		return End();
	targetLocation = ownerPtr->locationOnFight;
}

void PJExplosion::_Tick(float deltaTime)
{
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);

	auto distance = data->location - targetLocation;
	auto distancePow = pow(data->location.x - targetLocation.x, 2) + pow(data->location.y - targetLocation.y, 2);
	
	if (data->location.distance(targetLocation) < data->speed * deltaTime) {
		DoAttack();
		End();
	}
	else {
		data->direction = (targetLocation - data->location).Normalize(1);
		auto nextLocation = data->location + data->direction * data->speed * deltaTime;
		data->location = nextLocation;
	}
}

void PJExplosion::_End()
{
	ProjectileAct::_End();
}

void PJExplosion::DoAttack()
{
	auto ownerData = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);
	auto skillData = SkillData::GetSkillTemplate(data->templateID);
	auto explosionRange = 0;
	if (skillData == nullptr || ownerData == nullptr) return;
	for (auto& effect : skillData->levelData[ownerData->GetLevel()].effects)
		if (effect.first == SkillEffect::EXPLOSIONRANGE)
			explosionRange = effect.second;
	auto xmin = targetLocation.x - explosionRange-0.1;
	auto xmax = targetLocation.x + explosionRange+0.1;
	auto ymin = targetLocation.y - explosionRange-0.1;
	auto ymax = targetLocation.y + explosionRange+0.1;
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);
	if (ownerPtr == nullptr) return;
	//UE_LOG(Enoch, Warning, TEXT("doAttack %f %f -> %f %f begin"), xmin, ymin, xmax, ymax);
	for (auto& FL_index : *EnochSimulator::instance->FreeLancerQueue)
	{
		auto freelancer = EnochActorDataFactory::instance->GetFreeLancerData(FL_index);
		if (freelancer == nullptr) continue;
		if (ownerPtr->isEnemy == freelancer->isEnemy) continue;
		if (xmin <= freelancer->locationOnFight.x && freelancer->locationOnFight.x <= xmax &&
			ymin <= freelancer->locationOnFight.y && freelancer->locationOnFight.y <= ymax)
			FLBattleData::DoAttack(data, freelancer);
	}
	//UE_LOG(Enoch, Warning, TEXT("doAttack %f %f -> %f %f end"), xmin, ymin, xmax, ymax);
}