// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<vector>
#include<functional>
#include<memory>

using namespace std;

/*
 * 유니티 StateMachineBehaviour처럼 동작하는게 목표
 * 모든 Action 클래스는 이 클래스를 상속하여 _Begin, _Tick, _End 중 필요한 함수를 Override
 * 생성한 클래스는 FreeLancer :: actMap의 원하는 위치에 할당
 * 현재 각 Action의 동작시간은 length만큼의 시간으로 고정됨
 */
class FreeLancerAct
{
public:
	void Begin();
	void Tick(float deltaTime);
	void End();
	class FLBattleData* data;
	float length;
protected :
	virtual void _Begin();
	virtual void _Tick(float deltaTime);
	virtual void _End();
	float time;
private :
	//avoid Begin/End call twice
	bool _released = false;
	bool _initialized = false;
};