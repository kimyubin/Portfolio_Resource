// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFreeLancerAct.h"
#include "../EnochFreeLancerData.h"

using namespace std;

//unique_ptr<vector<function<bool(FLBattleData*)>>> FreeLancerAct::functionMap;
//EnochSimulator* FreeLancerAct::simulator;

void FreeLancerAct::Begin()
{
	if (!_initialized) {
		_initialized = true;
		_released = false;
		time = 0;
		_Begin();
	}
}

void FreeLancerAct::Tick(float deltaTime)
{
	time += deltaTime;
	data->UpdateTime(deltaTime);
	_Tick(deltaTime);
}

void FreeLancerAct::End()
{
	if (!_released) {
		_released = true;
		_initialized = false;
		_End();
	}
}

void FreeLancerAct::_Begin()
{
	;
}

void FreeLancerAct::_Tick(float deltaTime)
{
	;
}

void FreeLancerAct::_End()
{
	;
}