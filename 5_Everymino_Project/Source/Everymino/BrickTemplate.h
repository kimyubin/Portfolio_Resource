// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <deque>
#include "EMDefinedType.h"
#include "CoreMinimal.h"


using namespace std;
/**
 * 기본 브릭 형태 저장하는 클래스
 * 용어정리							<p>
 * Unit - 1칸짜리 블럭. 벽돌.			<p>
 * Brick - Mino. 유닛이 모여 만든 블럭. I,sq,t,s,z,j,L이 있음<p>
 * BrickShape - 블럭의 형상. 모양		<p>
 * UnitSkin - 1칸짜리 블럭 스킨		<p>
 */
class EVERYMINO_API BrickTemplate
{
public:
	//유닛 최대 개수. 0부터 계산. 4개면 0~3.
	static constexpr uint8 MAX_UNIT_NUM = 4;	
	static constexpr uint8 UNIT_SIZE = 50;		//한칸 사이즈 50

	BrickTemplate();
	~BrickTemplate();
	static vector<vector<vector<vector<UnitSkin>>>> GetBrickShapes(){return BrickShapes;}
	static deque<BrickType> GetShuffleBag(){return ShuffleBag;}
	static FVector2I GetWallKickData(BrickType InBrickType, BrickDirection InDirection,EMInput Rotation ,uint8 TryNum, EDropStartDirection InStartDirection);
	
	
private:
	static vector<vector<vector<vector<UnitSkin>>>> BrickShapes;
	static deque<BrickType> ShuffleBag;
	static vector<vector<FVector2I>> WallKickData;
	static vector<vector<FVector2I>> I_WallKickData;
	
};
