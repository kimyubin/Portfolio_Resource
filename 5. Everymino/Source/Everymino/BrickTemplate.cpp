// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BrickTemplate.h"

BrickTemplate::BrickTemplate()
{	
}

BrickTemplate::~BrickTemplate()
{
}

deque<BrickType> BrickTemplate::ShuffleBag = {
	BrickType::I, BrickType::Sq, BrickType::T, BrickType::S, BrickType::Z, BrickType::J, BrickType::L,
	BrickType::I, BrickType::Sq, BrickType::T, BrickType::S, BrickType::Z, BrickType::J, BrickType::L
};

FVector2I BrickTemplate::GetWallKickData(BrickType InBrickType, BrickDirection InDirection, EMInput Rotation, uint8 TryNum, EDropStartDirection InStartDirection)
{
	FVector2I res;

	/**
	 * 시계, 반시계 방향 2개씩이라 2배. 반시계면 짝수번. 
	 * 시작 위치가 시계방향으로 이동할 때마다, "UP"의 위치가 1씩 커져서 BrickDirection이 달라지므로,
	 * 시작 위치를 빼줘서 상쇄.
	 * 음수 방지.
	 */
	int MaxSize = EnumToInt(BrickDirection::Size);
	int rotationState = (((EnumToInt(InDirection) - EnumToInt(InStartDirection)) + MaxSize) % MaxSize) * 2;
	
	if (Rotation == EMInput::Clockwise)
		rotationState++;

	if (InBrickType == BrickType::I)
		res = I_WallKickData[rotationState][TryNum];
	else
		res = WallKickData[rotationState][TryNum];

	switch (InStartDirection)
	{
	case EDropStartDirection::North:
		break;
	case EDropStartDirection::East: //x, y축 교환
		swap(res.X,res.Y);
		break;
	case EDropStartDirection::South: //x,y 부호 반전
		res.X *= -1;
		res.Y *= -1;
		break;
	case EDropStartDirection::West: //x,y축 교환, 부호 반전
		swap(res.X,res.Y);
		res.X *= -1;
		res.Y *= -1;
		break;
	default:
		break;
	}
	return res;	
}

vector<vector<FVector2I>> BrickTemplate::WallKickData =
{
	//Try 0,		Try 1,			Try 2,			  Try 3,		  Try 4
	{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}, //03
	{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}, //01
	{{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}, //10
	{{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}, //12
	{{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}, //21
	{{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}, //23
	{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}, //32
	{{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}, //30
};

vector<vector<FVector2I>> BrickTemplate::I_WallKickData =
{
	//Try 0,		Try 1,			Try 2,			  Try 3,		  Try 4
	{{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}}, //03
	{{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}}, //01
	{{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}}, //10
	{{0, 0}, {-1, 0}, {+2, 0}, {-1, -2}, {+2, +1}}, //12
	{{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}}, //21
	{{0, 0}, {+2, 0}, {-1, 0}, {+2, -1}, {-1, +2}}, //23
	{{0, 0}, {-2, 0}, {+1, 0}, {-2, +1}, {+1, -2}}, //32
	{{0, 0}, {+1, 0}, {-2, 0}, {+1, +2}, {-2, -1}}, //30
};

vector<vector<vector<vector<UnitSkin>>>> BrickTemplate::BrickShapes = {
	{
		{
			{US_E, US_E, US_E, US_E},
			{US_D, US_D, US_D, US_D},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_D, US_E},
			{US_E, US_E, US_D, US_E},
			{US_E, US_E, US_D, US_E},
			{US_E, US_E, US_D, US_E}
		},
		{
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E},
			{US_D, US_D, US_D, US_D},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E}
		}
	},
	{
		{
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		}
	},
	{
		{
			{US_E, US_D, US_E, US_E},
			{US_D, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_E, US_E},
			{US_D, US_D, US_D, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_D, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		}
	},
	{
		{
			{US_E, US_D, US_D, US_E},
			{US_D, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_D, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_E, US_E},
			{US_E, US_D, US_D, US_E},
			{US_D, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_D, US_E, US_E, US_E},
			{US_D, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		}
	},
	{
		{
			{US_D, US_D, US_E, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_D, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_E, US_E},
			{US_D, US_D, US_E, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_D, US_D, US_E, US_E},
			{US_D, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		}
	},
	{
		{
			{US_E, US_E, US_D, US_E},
			{US_D, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_E, US_E},
			{US_D, US_D, US_D, US_E},
			{US_D, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_D, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		}
	},
	{
		{
			{US_D, US_E, US_E, US_E},
			{US_D, US_D, US_D, US_E},
			{US_E, US_E, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_D, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_E, US_E, US_E},
			{US_D, US_D, US_D, US_E},
			{US_E, US_E, US_D, US_E},
			{US_E, US_E, US_E, US_E}
		},
		{
			{US_E, US_D, US_E, US_E},
			{US_E, US_D, US_E, US_E},
			{US_D, US_D, US_E, US_E},
			{US_E, US_E, US_E, US_E}
		}
	}
};
