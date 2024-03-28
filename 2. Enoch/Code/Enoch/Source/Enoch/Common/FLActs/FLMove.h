// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include"EnochFreeLancerAct.h"
#include "../EnochGeometry.h"
#include<vector>
#include<functional>
#include<memory>

class FLMove : public FreeLancerAct
{
public:
	void _Begin();
	void _End();
	void _Tick(float deltaTime);
	Vector2D prevLocation;
	Vector2D postLocation;
	VectorF2D direction;
	VectorF2D floatLocation;
protected:
};