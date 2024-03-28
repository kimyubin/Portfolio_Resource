#include "PJLinearArrow.h"
#include "../EnochProjectileData.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActorDataFactory.h"
#include "../EnochSimulator.h"
#include "../../Enoch.h"

void PJLinearArrow::_Begin()
{
	freelancer_attacked = std::set<int>();
	length = 10;
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);
	auto targetPtr = EnochActorDataFactory::instance->GetFreeLancerData(target);
	if (targetPtr == nullptr || ownerPtr == nullptr) 
		return End();
	//fixed attack range to 15
	targetLocation = ownerPtr->locationOnFight + VectorF2D(targetPtr->locationOnFight - ownerPtr->locationOnFight).Normalize(15);
}

void PJLinearArrow::_Tick(float deltaTime)
{
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);

	auto distance = data->location - targetLocation;
	auto distancePow = pow(data->location.x - targetLocation.x, 2) + pow(data->location.y - targetLocation.y, 2);
	
	if (data->location.distance(targetLocation) < data->speed * deltaTime) {
		DoAttack(Segment(data->location, targetLocation, width));
		End();
	}
	else {
		data->direction = (targetLocation - data->location).Normalize(1);
		auto nextLocation = data->location + data->direction * data->speed * deltaTime;
		DoAttack(Segment(data->location, nextLocation, width));
		data->location = nextLocation;
	}
}

void PJLinearArrow::_End()
{
	ProjectileAct::_End();
}

void PJLinearArrow::DoAttack(Segment segment)
{
	auto xmin = std::min(segment.src.x, segment.dst.x) - (width + 0.4);
	auto xmax = std::max(segment.src.x, segment.dst.x) + (width + 0.4);
	auto ymin = std::min(segment.src.y, segment.dst.y) - (width + 0.4);
	auto ymax = std::max(segment.src.y, segment.dst.y) + (width + 0.4);
	auto ownerPtr = EnochActorDataFactory::instance->GetFreeLancerData(data->_owner);
	if (ownerPtr == nullptr) return;
	//UE_LOG(Enoch, Warning, TEXT("doAttack %f %f -> %f %f begin"), xmin, ymin, xmax, ymax);
	for (auto& FL_index : *EnochSimulator::instance->FreeLancerQueue)
	{
		if (freelancer_attacked.find(FL_index) != freelancer_attacked.end()) continue;
		auto freelancer = EnochActorDataFactory::instance->GetFreeLancerData(FL_index);
		if (freelancer == nullptr) continue;
		if (ownerPtr->isEnemy == freelancer->isEnemy) continue;
		//UE_LOG(Enoch, Warning, TEXT("freelancer %d %d"), freelancer->locationOnFight.x, freelancer->locationOnFight.y);
		if (xmin <= freelancer->locationOnFight.x && freelancer->locationOnFight.x <= xmax &&
			ymin <= freelancer->locationOnFight.y && freelancer->locationOnFight.y <= ymax &&
			isCollided(freelancer->getCollidCircle(), segment)) {
			//UE_LOG(Enoch, Warning, TEXT("collided freelancer %d %d"), freelancer->locationOnFight.x, freelancer->locationOnFight.y);
			FLBattleData::DoAttack(data, freelancer);
			freelancer_attacked.insert(FL_index);
		}
	}
	//UE_LOG(Enoch, Warning, TEXT("doAttack %f %f -> %f %f end"), xmin, ymin, xmax, ymax);
}