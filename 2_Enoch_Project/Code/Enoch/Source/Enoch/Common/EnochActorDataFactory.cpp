// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActorDataFactory.h"
#include "EnochFreeLancerData.h"
#include "EnochProjectileData.h"
#include "EnochFieldCellData.h"
#include "SkillData.h"

EnochActorDataFactory* EnochActorDataFactory::instance;

EnochActorDataFactory::EnochActorDataFactory()
{
	actorID = 0;
}

EnochActorDataFactory::~EnochActorDataFactory()
{
}

void EnochActorDataFactory::BeginPlay()
{
	_FreeLancerMap.clear();
	_ProjectileMap.clear();
	allianceAlly.clear();
	allianceEnemy.clear();
	_SerialNumber_FreeLancer = 1;
	_SerialNumber_Projectile = 1;
	instance = this;
}
  
int EnochActorDataFactory::SpawnFreeLancer()
{
	_FreeLancerMap[_SerialNumber_FreeLancer] = std::make_shared<FLBattleData>(_SerialNumber_FreeLancer);
	return _SerialNumber_FreeLancer++;
}

int EnochActorDataFactory::SpawnProjectile()
{
	_ProjectileMap[_SerialNumber_Projectile] = std::make_shared<EnochProjectileData>(_SerialNumber_Projectile);
	return _SerialNumber_Projectile++;
}

void EnochActorDataFactory::DeleteFreeLancer(int SerialNumber)
{
	_FreeLancerMap.erase(SerialNumber);
}


void EnochActorDataFactory::DeleteProjectile(int SerialNumber)
{
	_ProjectileMap.erase(SerialNumber);
}

void EnochActorDataFactory::EndPlay()
{
	_FreeLancerMap.clear();
	_ProjectileMap.clear();
	allianceAlly.clear();
	allianceEnemy.clear();

	instance = nullptr;
}

void EnochActorDataFactory::UpdateAllianceAll()
{
	allianceAlly.clear();
	allianceEnemy.clear();

	// 삽입
	for (auto& elem : _FreeLancerMap)
	{
		auto& data = elem.second;
		auto& alliMap = data->isEnemy ? allianceEnemy : allianceAlly;

		auto tmp = FreeLancerTemplate::GetFreeLancerTemplate(data->GetTID());
		if (tmp == nullptr)
			continue;
		for (auto& alliID : tmp->alliance)
		{
			if (alliMap.find(alliID) == alliMap.end())
			{
				// Level, MemberSet
				alliMap[alliID] = make_pair(0, make_shared<set<uint8_t>>() );
			}

			alliMap[alliID].second->insert(tmp->ID);
		}
	}

	// 용병에 효과 넣기
	// Scope는 총 4개 : AllyAlliance, AllyAll, Enemy, EnemyAll
	enum ScopeTemp {
		AllyAlliance,
		AllyAll,
		EnemyAlliance,
		EnemyAll,
		Count,
	};
	typedef pair<uint8_t, uint8_t>	alliPair;	// alliance, level
	array<vector<alliPair>, ScopeTemp::Count>	effectList;	// [Scope][alliPair]

	// 멤버 수 확인하고 레벨 설정
	auto levelSet = [&](auto &alliMap, wstring& str, bool isEnemy)->void
	{
		for (auto& elem : alliMap)
		{
			auto alliID = elem.first;
			uint8_t count = elem.second.second->size();
			auto alliTmp = AllianceTemplate::GetAllianceTemplate(alliID);
			uint8_t level = 0;
			for (auto& elemSub : alliTmp->levelData)
			{
				auto& lv = elemSub.first;
				if (count < lv)
					break;
				level++;
			}
			elem.second.first = level;
			if (level > 0)
			{
				if (!str.empty()) str += '\n';
				str += alliTmp->name + L"(" + to_wstring(level) + L")";

				// 동맹 별 스코프에 넣기 처리
				AllianceScope scope = alliTmp->levelData[level - 1].second;
				if (!isEnemy)
				{
					switch (scope) {
					case AllianceScope::Alliance:
						effectList[ScopeTemp::AllyAlliance].push_back(make_pair(alliID, level));
						break;
					case AllianceScope::AllyAll:
						effectList[ScopeTemp::AllyAll].push_back(make_pair(alliID, level));
						break;
					case AllianceScope::EnemyAll:
						effectList[ScopeTemp::EnemyAll].push_back(make_pair(alliID, level));
						break;
					}
				}
				else {
					switch (scope) {
					case AllianceScope::Alliance:
						effectList[ScopeTemp::EnemyAlliance].push_back(make_pair(alliID, level));
						break;
					case AllianceScope::AllyAll:
						effectList[ScopeTemp::EnemyAll].push_back(make_pair(alliID, level));
						break;
					case AllianceScope::EnemyAll:
						effectList[ScopeTemp::AllyAll].push_back(make_pair(alliID, level));
						break;
					}
				}
			}
		}
	};

	strAlly.clear();
	strEnemy.clear();
	levelSet(allianceAlly, strAlly, false);
	levelSet(allianceEnemy, strEnemy, true);

	// 용병 별 스코프에 맞춰 처리
	auto insertSkill = [&](auto& elem, auto& data, auto& tmp, bool ally) {
		uint8_t alliID = elem.first;
		uint8_t level = elem.second;
		auto alliTmp = AllianceTemplate::GetAllianceTemplate(alliID);
		if (ally) {
			bool find = false;
			for(auto alliID_ : tmp->alliance) {
				if (alliID == alliID_) {
					find = true;
					break;
				}
			}
			if (!find)
				return;
		}

		data->affectedList[(int)AffectedSituation::Always].push_back(
			AffectedSkill(
				alliTmp->skillID, level,
				AffectedReason::Alliance, alliID
			)
		);
	};

	for (auto& elem : _FreeLancerMap) {
		auto& data = elem.second;
		auto tmp = FreeLancerTemplate::GetFreeLancerTemplate(data->GetTID());
		data->affectedList[(int)AffectedSituation::Always].clear();
		if (!data->isEnemy) {
			for (auto& elem_ : effectList[ScopeTemp::AllyAll])
				insertSkill(elem_, data, tmp, false);
			for (auto& elem_ : effectList[ScopeTemp::AllyAlliance])
				insertSkill(elem_, data, tmp, true);
		}
		else {
			for (auto& elem_ : effectList[ScopeTemp::EnemyAll])
				insertSkill(elem_, data, tmp, false);
			for (auto& elem_ : effectList[ScopeTemp::EnemyAlliance])
				insertSkill(elem_, data, tmp, true);
		}
		data->UpdateBaseStat();
	}
}

void EnochActorDataFactory::UpdateFLAll() {
	for (auto &fl : _FreeLancerMap) {
		fl.second->UpdateStat();
	}
}

uint8_t EnochActorDataFactory::GetAllianceLevel(bool isEnemy, uint8_t allianceID)
{
	auto alli = this->GetAlliance(isEnemy);
	if (alli.find(allianceID) == alli.end())
		return 0;
	return alli.find(allianceID)->second.first;
}