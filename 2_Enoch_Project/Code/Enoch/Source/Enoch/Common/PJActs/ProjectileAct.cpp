// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileAct.h"
#include "../EnochProjectileData.h"
#include "../EnochSimulator.h"
#include "../EnochActorDataFactory.h"

using namespace std;

//unique_ptr<vector<function<bool(FLBattleData*)>>> FreeLancerAct::functionMap;
//EnochSimulator* FreeLancerAct::simulator;

void ProjectileAct::Begin()
{
	if (!_initialized) {
		_initialized = true;
		_released = false;
		time = 0;
		_Begin();
	}
}

void ProjectileAct::Tick(float deltaTime)
{
	if (!_released) {
		time += deltaTime;
		data->UpdateTime(deltaTime);
		_Tick(deltaTime);
		if (time > length)
			End();
	}
}

void ProjectileAct::End()
{
	if (!_released) {
		_released = true;
		_initialized = false;
		_End();
	}
}

void ProjectileAct::_Begin()
{
	;
}

void ProjectileAct::_Tick(float deltaTime)
{
	;
}

void ProjectileAct::_End()
{
	EnochSimulator::logs.push({ SimulateInfoType::RemovePJ, data->SerialNumber });
	data->expired = true;
}