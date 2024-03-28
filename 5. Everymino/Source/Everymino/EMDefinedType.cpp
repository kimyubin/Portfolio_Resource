// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMDefinedType.h"


FKey EBindKey::MoveUp = EKeys::Up;
FKey EBindKey::MoveRight = EKeys::Right;
FKey EBindKey::MoveDown = EKeys::Down;
FKey EBindKey::MoveLeft = EKeys::Left;

FKey EBindKey::HardDrop = EKeys::SpaceBar;

FKey EBindKey::Clockwise = EKeys::C;
FKey EBindKey::AntiClockwise = EKeys::X;

FKey EBindKey::SelectNorth = EKeys::W;
FKey EBindKey::SelectEast = EKeys::D;
FKey EBindKey::SelectSouth = EKeys::S;
FKey EBindKey::SelectWest = EKeys::A;


const FVector2I FVector2I::ZeroVector(0, 0);

FVector2I FVector2I::operator+(const FVector2I& V) const
{
	return FVector2I(X + V.X, Y + V.Y);
}

FVector2I FVector2I::operator+=(const FVector2I& V)
{
	X += V.X; Y += V.Y;
	return *this;
}
