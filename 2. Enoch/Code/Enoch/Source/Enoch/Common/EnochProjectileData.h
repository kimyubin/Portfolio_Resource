// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "EnochGeometry.h"

using namespace std;

enum class ProjectileTemplateID : uint8 {
	None = 0, //NULL
    RedDot = 1,
	TypeNum,
};
/**
 * 
 */
class EnochProjectileData
{
public:
	EnochProjectileData(int SerialNumberIn) : SerialNumber(SerialNumberIn) {};
	~EnochProjectileData();
	void Release();
	void Act(float deltaTime);

	void Set(int damageIn, float speedIn, int target, int owner, SkillData *skillData = nullptr);

	int attackDamage;
	bool expired = false;	//if true, deleted next tick
	int templateID = -1;
	int SerialNumber;
	shared_ptr<class ProjectileAct> act;
	void UpdateTime(float deltaTime);
public:
	float speed; //투사체 속도
	VectorF2D location;
	VectorF2D direction;
	int _owner;	//투사체 발사자
	
	//각종 버프, 디버프는 나중에 추가.
};
