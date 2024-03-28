// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochProjectileData.h"
#include "EnochActorDataFactory.h"
#include "EnochFreeLancerData.h"
#include "PJActs/PJGuidedArrow.h"
#include "PJActs/PJLinearArrow.h"
#include "PJActs/PJExplosion.h"


EnochProjectileData::~EnochProjectileData()
{
	
}

void EnochProjectileData::Set(int damageIn, float speedIn, int target, int owner, SkillData *skillData) {
	attackDamage = damageIn;
	speed = speedIn;
	_owner = owner;
	auto ownerData = EnochActorDataFactory::instance->GetFreeLancerData(_owner);

	location = VectorF2D(ownerData->locationOnFight);

	if (skillData != nullptr)
		templateID = skillData->ID;
	//탄환에도 타입이 필요함. 유도, 비유도, 관통, 범위 등
	if (skillData == nullptr || skillData->target == SkillTarget::Enemy_Single)
		act = make_shared<PJGuidedArrow>();
	else if (skillData->target == SkillTarget::Enemy_Linear) {
		auto _act = make_shared<PJLinearArrow>();
		for (auto &effect : skillData->levelData[ownerData->GetLevel()].effects)
			if (effect.first == SkillEffect::SKILLWIDTH)
				_act->width = effect.second;
		act = _act;
	}
	else if (skillData->target == SkillTarget::Enemy_Around) {
		auto _act = make_shared<PJExplosion>();
		act = _act;
	}

	act->data = this;
	act->target = target;
	
	act->Begin();
}

void EnochProjectileData::Act(float deltaTime)
{
	act->Tick(deltaTime);
}

void EnochProjectileData::UpdateTime(float deltaTime)
{
	//추후 시간에 따른 데이터 변화 적용을 위해 만들어만 둔 함수
}