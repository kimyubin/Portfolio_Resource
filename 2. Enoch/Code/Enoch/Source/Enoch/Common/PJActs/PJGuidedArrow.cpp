#include "PJGuidedArrow.h"
#include "../EnochProjectileData.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochSimulator.h"

void PJGuidedArrow::_Begin()
{
	length = 10;
}

void PJGuidedArrow::_Tick(float deltaTime)
{
	auto targetPtr = EnochActorDataFactory::instance->GetFreeLancerData(target);
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);
	auto targetLoc = targetPtr->locationOnFight;

	auto distance = data->location - targetLoc;
	auto distancePow = pow(data->location.x - targetLoc.x, 2) + pow(data->location.y - targetLoc.y, 2);

	if (data->location.distance(targetLoc) < data->speed * deltaTime) {
		End();
	}
	else {
		data->direction = (targetLoc - data->location).Normalize(1);
		data->location = data->location + data->direction * data->speed * deltaTime;
	}
}

void PJGuidedArrow::_End()
{
	ProjectileAct::_End();
	auto targetPtr = EnochActorDataFactory::instance->GetFreeLancerData(target);
	if (targetPtr != nullptr)
		FLBattleData::DoAttack(data,  targetPtr);
}