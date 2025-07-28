// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFieldCellData.h"
#include "EnochFreeLancerData.h"

EnochFieldCellData::EnochFieldCellData(int inputY, int inputX)
	: location({ inputY,inputX }), FreeLancerOn(-1), FreeLancerOnFight(-1)
{
};

EnochFieldCellData::~EnochFieldCellData()
{
}


void EnochFieldCellData::SetFreeLancerOn(FLBattleData* arg)
{
	FreeLancerOn = arg->GetSerialNumber();
	if(arg != nullptr) arg->location = location;
}

void EnochFieldCellData::SetFreeLancerOnFight(FLBattleData* arg)
{
	FreeLancerOnFight = arg->GetSerialNumber();
}

void EnochFieldCellData::SetFreeLancerOnFight(int SN)
{
	FreeLancerOnFight = SN;
}