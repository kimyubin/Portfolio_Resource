// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<vector>
#include<functional>
#include<memory>

using namespace std;

/*
 * 유니티 StateMachineBehaviour처럼 동작하는게 목표
 * 모든 Action 클래스는 이 클래스를 상속하여 _Begin, _Tick, _End 중 필요한 함수를 Override
 * 생성한 클래스는 Projectile :: actMap의 원하는 위치에 할당
 * 각 동작간 전이가 현재 외부에서 일어나지 않으므로, Tick()함수에 로직 종료 판정이 들어가있어야함
 */
class ProjectileAct
{
public:
	void Begin();
	void Tick(float deltaTime);
	void End();
	class EnochProjectileData* data;
	float length;

	int target;	//투사체 타겟. targetLocation과 동시에 사용되지 않음
	VectorF2D targetLocation;	//투사체 타겟지점. target과 동시에 사용되지 않음
protected:
	virtual void _Begin();
	virtual void _Tick(float deltaTime);
	virtual void _End();
	float time;
private:
	//avoid Begin/End call twice
	bool _released = false;
	bool _initialized = false;
};