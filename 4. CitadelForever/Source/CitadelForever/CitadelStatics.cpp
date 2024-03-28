// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelStatics.h"

#include "CitadelForeverTypes.h"

using namespace std;
double UCitadelStatics::CalPlaneDistance(FVector StartPoint, FVector EndPoint)
{
	double xDis = abs(StartPoint.X - EndPoint.X);
	double yDis = abs(StartPoint.Y - EndPoint.Y);

	return sqrt(pow(xDis,2)+pow(yDis,2));
}
double UCitadelStatics::RoundUpForUnit(double InValue)
{
	const double remain = std::fmod(InValue, CitadelConstant::Unit_Range);

	if (InValue > 0)
		InValue += (CitadelConstant::Unit_Range / 2);
	else
		InValue -= (CitadelConstant::Unit_Range / 2);

	InValue -= remain;
	return InValue;
}
