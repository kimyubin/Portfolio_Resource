// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFiniteStateMachine.h"
#include "EnochFieldData.h"
#include "EnochActorDataFactory.h"
#include "EnochFieldCellData.h"
#include "EnochFreeLancerState.h"
#include "EnochFreeLancerData.h"
#include "EnochSimulator.h"

#include "FLActs/FLMove.h"
#include "FLActs/FLJump.h"

#include <algorithm>
#include <queue>
#include <vector>

#include "../Enoch.h"
void EnochFiniteStateMachine::Decide(FLBattleData *freeLancer)
{
	if (freeLancer == nullptr) return;
	auto canSpell = true;
	auto canAttack = true;
	auto canJump = true;
	auto canMove = true;
	for (auto& affected_elem : freeLancer->affectedList[(int)AffectedSituation::State])
	{
		if ((SkillEffect)affected_elem.reasonID == SkillEffect::STUN) {
			canSpell = false;
			canAttack = false;
			canJump = false;
			canMove = false;
		}
	}
	if (!canSpell || !trySpell(freeLancer))
		if (!canAttack || !tryAttack(freeLancer))
			if (!canJump || !tryJump(freeLancer))
				if(!canMove || !tryMove(freeLancer))
					freeLancer->SetState(FreeLancerState::Idle);
}

bool EnochFiniteStateMachine::trySpell(FLBattleData* freeLancer){
	if (freeLancer->mpMax - freeLancer->mpNow > 0.01) return false;
	auto skill = SkillData::GetSkillTemplate(freeLancer->skill);
	if (skill == nullptr || skill->shape == SkillShape::Passive) return false;
	{
		auto skillRange = freeLancer->attackRange;
		for (auto e : skill->levelData[freeLancer->GetLevel()].effects)
			if (e.first == SkillEffect::SKILLRANGE)
				skillRange = (int)e.second;
		auto targetLocation = GetTarget(freeLancer, [isEnemy = freeLancer->isEnemy](FLBattleData* freeLancer2){
			return freeLancer2->isEnemy != isEnemy && freeLancer2->GetState() != FreeLancerState::Dead;
		}, true);
		auto cellData = EnochFieldData::GetData(targetLocation);
		if (cellData == nullptr)
			return false;

		auto targetIndex = cellData->FreeLancerOnFight;
		auto target = EnochActorDataFactory::instance->GetFreeLancerData(targetIndex);

		//타겟 결정 실패
		if (target == nullptr)
			return false;
		freeLancer->SetTarget(target);
		//공격이 가능한 거리인경우 공격으로 결정
		if (skillRange >= CalculateDistance(freeLancer->locationOnFight, target->locationOnFight)) {
			//return false;
			//if (freeLancer->CanAttack())
				freeLancer->SetState(FreeLancerState::PreSkill);
			//else
			//	freeLancer->SetState(FreeLancerState::Idle);
			//쿨타임때문에 가만히 있는 경우
			return true;
		}

	}
	return false;
	//skill->levelData[0].effects
}

bool EnochFiniteStateMachine::tryAttack(FLBattleData* freeLancer)
{
	auto targetLocation = GetTarget(freeLancer, [isEnemy = freeLancer->isEnemy](FLBattleData* freeLancer2){
		return freeLancer2->isEnemy != isEnemy && freeLancer2->GetState() != FreeLancerState::Dead;
	}, true);
	auto cellData = EnochFieldData::GetData(targetLocation);
	if (cellData == nullptr) {
		freeLancer->SetTarget(nullptr);
		return false;
	}
	auto targetIndex = cellData->FreeLancerOnFight;
	auto target = EnochActorDataFactory::instance->GetFreeLancerData(targetIndex);
	
	//타겟 결정 실패
	if (target == nullptr)
		return false;

	freeLancer->SetTarget(target);
	//공격이 가능한 거리인경우 공격으로 결정
	if (freeLancer->attackRange >= CalculateDistance(freeLancer->locationOnFight, target->locationOnFight)) {
		if (freeLancer->CanAttack()) {
			freeLancer->nextAttackTime = EnochSimulator::simulateTime + freeLancer->timeForOneAttack;
			freeLancer->SetState(FreeLancerState::PreAttack);
		}
		else
			freeLancer->SetState(FreeLancerState::Idle);
		//쿨타임때문에 가만히 있는 경우
		return true;
	}
	return false;
}

bool EnochFiniteStateMachine::tryJump(FLBattleData* freeLancer)
{
	if (!freeLancer->canJump) return false;
	auto targetLocation = GetTarget(freeLancer, [isEnemy = freeLancer->isEnemy](FLBattleData* freeLancer2){
		return freeLancer2->isEnemy != isEnemy && freeLancer2->GetState() != FreeLancerState::Dead;
	},false);
	auto cellData = EnochFieldData::GetData(targetLocation);
	if (cellData == nullptr) {
		return false;
	}
	auto targetIndex = cellData->FreeLancerOnFight;
	auto target = EnochActorDataFactory::instance->GetFreeLancerData(targetIndex);
	freeLancer->SetTarget(target);
	
	if (target == nullptr) return false;
	auto jumpTargetLocation = FindJumpPath(freeLancer, target);
	auto rawActInst = freeLancer->actMap->at((int)FreeLancerState::PreJump);

	auto jumpInst = static_pointer_cast<FLJump>(rawActInst);
	jumpInst->prevLocation = freeLancer->locationOnFight;
	jumpInst->postLocation = jumpTargetLocation;

	auto targetCellData = EnochFieldData::GetData(jumpTargetLocation);
	targetCellData->FreeLancerOnFight = freeLancer->GetSerialNumber();
	freeLancer->SetState(FreeLancerState::PreJump);
	return true;
}

bool EnochFiniteStateMachine::tryMove(FLBattleData* freeLancer)
{
	auto target = freeLancer->GetTarget();
	if (target == nullptr)
		return false;
	auto moveTargetLocation = FindPath(freeLancer, target->locationOnFight);

	auto rawActInst = freeLancer->actMap->at((int)FreeLancerState::PreMove);
	auto moveInst = static_pointer_cast<FLMove>(rawActInst);
	moveInst->prevLocation = freeLancer->locationOnFight;
	moveInst->postLocation = moveTargetLocation;

	auto targetCellData = EnochFieldData::GetData(moveTargetLocation);
	targetCellData->FreeLancerOnFight = freeLancer->GetSerialNumber();
	freeLancer->SetState(FreeLancerState::PreMove);
	return true;
}

//get closest target
Vector2D EnochFiniteStateMachine::GetTarget(FLBattleData *freeLancer, std::function<bool(FLBattleData*)>constraint, bool closest)
{
	auto xNow = freeLancer->locationOnFight.x;
	auto yNow = freeLancer->locationOnFight.y;
	EnochFieldCellData* cell;

	//////////////////////////////////////////
#define CHECKCELL(yy, xx) \
	cell = EnochFieldData::GetData({ yy , xx }); \
	if (cell) { \
		freeLancer2 = EnochActorDataFactory::instance->GetFreeLancerData(cell->FreeLancerOnFight); \
		if (freeLancer2 != nullptr && constraint(freeLancer2)) \
			if (!freeLancer->canJump || FindJumpPath(freeLancer, freeLancer2).x > 0) \
				return { yy, xx }; \
	}
	
	//////////////////////////////////////////

	FLBattleData* freeLancer2 = nullptr;
	auto begin = 1;
	auto end = 10;
	auto offset = 1;
	if (!closest) {
		begin = 10;
		end = 1;
		offset = -1;
	}
	for (int distance = begin; distance != end; distance += offset)
	{
		//90n degree check
		CHECKCELL(yNow + distance , xNow);
		CHECKCELL(yNow - distance , xNow);
		CHECKCELL(yNow , xNow + distance);
		CHECKCELL(yNow , xNow - distance);

		//90n + (-44 ~ 44) degree check
		for (int slope = 1; slope < distance; slope++)
		{
			CHECKCELL(yNow + distance, xNow + slope);
			CHECKCELL(yNow + distance, xNow - slope);
			
			CHECKCELL(yNow - distance, xNow + slope);
			CHECKCELL(yNow - distance, xNow - slope);
			
			CHECKCELL(yNow + slope, xNow - distance);
			CHECKCELL(yNow - slope, xNow - distance);
			
			CHECKCELL(yNow + slope, xNow + distance);
			CHECKCELL(yNow - slope, xNow + distance);
		}

		//90n + 45 degree check
		CHECKCELL(yNow + distance, xNow + distance);
		CHECKCELL(yNow - distance, xNow + distance);
		CHECKCELL(yNow + distance, xNow - distance);
		CHECKCELL(yNow - distance, xNow - distance);
	}
	//no result
	return { -1,-1 };
}

inline int EnochFiniteStateMachine::CalculateDistance(Vector2D src, Vector2D dst)
{
	return std::max(
		std::abs(src.x - dst.x),
		std::abs(src.y - dst.y)
		);
}

Vector2D EnochFiniteStateMachine::FindPath(FLBattleData* freeLancer, Vector2D dst)
{
	auto src = freeLancer->locationOnFight;
	auto q = std::queue<Vector2D>();
	
	auto checkMap = std::vector<std::vector<bool>>();
	//맵을 true로 초기화된 10x10 크기로 생성
	checkMap.resize(10,std::vector<bool>(10,true));
	checkMap[dst.y][dst.x] = false;
	q.push({dst.y, dst.x});

	////////////////////////////////////////////////////
	auto _CheckPath = [&checkMap = checkMap, &q = q](int y, int x) {
		auto data = EnochFieldData::GetData({ y , x });
		if (!data) return;
		auto freeLancerData = EnochActorDataFactory::instance->GetFreeLancerData(data->FreeLancerOnFight);
		if (data && checkMap.at(y).at(x) && (freeLancerData==nullptr || freeLancerData->state == FreeLancerState::Dead)) {
			checkMap[y][x] = false;
			q.push({ y , x });
		}
	};
#define CHECKPATH(yy, xx) \
	data = EnochFieldData::GetData({yy , xx}); \
	if (data && map[ yy ][ xx ] && data->FreeLancerOnFight == -1 ) { \
		map[ yy ][ xx ] = false; \
		q.push({yy , xx}); \
	}
	////////////////////////////////////////////////////

	//최단경로를 찾고 첫노드를 찾기는 어려우므로, 역방향 최단경로를 찾는 방법 사용
	//인게임에선 대각선이동이 가능하지만, 자연스럽게 이동하는걸로 보이려면 직선이동 기준으로 루트를 짜야함
	while (!q.empty())
	{
		auto node = q.front();
		q.pop();
		if (CalculateDistance(src, node) <= 1) return node;
		_CheckPath(node.y + 1, node.x);
		_CheckPath(node.y, node.x + 1);
		_CheckPath(node.y, node.x - 1);
		_CheckPath(node.y - 1, node.x);
	}

	//전부 불가능한 경우..
	return {src.y,src.x};
}

Vector2D EnochFiniteStateMachine::FindJumpPath(FLBattleData* freeLancer, FLBattleData* freeLancer2)
{
	auto src = freeLancer->locationOnFight;
	auto candidates = std::vector<Vector2D>();

	auto dst = freeLancer2->locationOnFight;
	
	//타깃을 공격 가능한 위치들이 후보군
	for(int y = dst.y - freeLancer->attackRange; y <= dst.y + freeLancer->attackRange; y++)
		for (int x = dst.x - freeLancer->attackRange; x <= dst.x + freeLancer->attackRange; x++)
		{
			auto cellData = EnochFieldData::GetData({ y,x });
			if (cellData == nullptr) continue;
			auto _freeLancer = EnochActorDataFactory::instance->GetFreeLancerData(cellData->FreeLancerOnFight);
			if (_freeLancer != nullptr && _freeLancer->state != FreeLancerState::Dead) continue;
			candidates.push_back({ y,x });
		}
	
	//후보군 중 현재 위치에서 가장 먼 지점을 뽑음 (맨해튼 거리)
	std::sort(candidates.begin(), candidates.end(), [src = src](Vector2D lhs, Vector2D rhs) {
		return abs(lhs.x - src.x) + abs(lhs.y - src.y) > abs(rhs.x - src.x) + abs(rhs.y - src.y);
		});
	return (candidates.begin() == candidates.end()) ? Vector2D{ -1,-1 } : candidates.front();
}
