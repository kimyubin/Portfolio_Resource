// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "EnochFieldData.h"
/*
 * 입력으로 필드정보와 프리랜서 정보를 주면, 다음동작을 결정하여 프리랜서의 상태를 변경시켜줄 클래스
 * 인스턴스 생성 없이 스태틱 함수들로만 구성할 것(클래스라기보단 namespace의 역할)
 * 실제 구현이 FSM으로 되어있지 않으므로, 추후 이름 변경해야함
 */
class EnochFiniteStateMachine
{
public:
	EnochFiniteStateMachine() = delete;
	
	static void Decide(class FLBattleData *freelancer);

private:
	
	/*
	* 가장 가깝거나 먼 목표물의 좌표를 얻음. 필요에 따라 constraint를 통해 조건을 걸 수도 있음.
	* contraint가 false를 리턴하는 목표물은 탐색에서 제외함
	* default는 모든 대상을 리턴하므로, 적어도 아군/적군을 가리는 constraint를 걸어야함
	*/
	static Vector2D GetTarget(FLBattleData *freeLancer, std::function<bool(FLBattleData *)>constraint = [](FLBattleData*) {return true; }, bool closest=true);
	
	static bool trySpell(FLBattleData* freeLancer);
	static bool tryAttack(FLBattleData* freeLancer);
	static bool tryJump(FLBattleData* freeLancer);
	static bool tryMove(FLBattleData* freeLancer);
	
	// 가장 먼 목표물의 좌표를 얻음
	//static Vector2D GetFarthest(FLBattleData* freeLancer);
	static inline int CalculateDistance(Vector2D src, Vector2D dst);
	static Vector2D FindPath(class FLBattleData* freeLancer, Vector2D dst);
	static Vector2D FindJumpPath(class FLBattleData* freeLancer, FLBattleData* freeLancer2);
};