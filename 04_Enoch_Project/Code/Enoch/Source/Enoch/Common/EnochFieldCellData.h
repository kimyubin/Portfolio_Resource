// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnochFieldData.h"
/**
 * 필드 중 한칸에 대한 데이터
 */
class EnochFieldCellData
{
public:
	EnochFieldCellData(int inputY, int inputX);
	~EnochFieldCellData();
	void SetFreeLancerOn(class FLBattleData *arg);
	void SetFreeLancerOnFight(class FLBattleData* arg);
	void SetFreeLancerOnFight(int SN);
	Vector2D GetLocation() { return location; }
private :
	Vector2D location;
	int FreeLancerOn;		//현재 이 셀을 밟고있는 프리랜서(비전투중)
	int FreeLancerOnFight;	//현재 이 셀을 밟고있는 프리랜서(전투중)
	friend class EnochSimulator;
	friend class EnochFiniteStateMachine;
	friend class EnochFreeLancerAct;
};

