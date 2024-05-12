// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusTypes.h"

#include "OmnibusUtilities.h"

const FName EOmniComponentTag::StartPoint_RoadSpline("StartPoint_RoadSpline");
const FName EOmniComponentTag::EndPoint_RoadSpline("EndPoint_RoadSpline");
const FName EOmniComponentTag::StartPoint_Lane("StartPoint_Lane");
const FName EOmniComponentTag::EndPoint_Lane("EndPoint_Lane");

const FName EOmniCollisionProfile::RoadConnector("RoadConnectorOverlap");
const FName EOmniCollisionProfile::SysDetector("SysDetectorOverlap");
const FName EOmniCollisionProfile::BusDetector("BusDetectorOverlap");

std::vector<FColor> EOmniColor::ColorSet = {
	FColor(204, 0, 0), FColor(255, 0, 0), FColor(255, 100, 0), FColor(255, 200, 0), FColor(255, 255, 0), FColor(160, 255, 0), FColor(0, 255, 0)
, FColor(0, 255, 186), FColor(0, 205, 255), FColor(0, 125, 255), FColor(0, 0, 255), FColor(81, 0, 255), FColor(162, 0, 255), FColor(145, 0, 158)
};

FColor EOmniColor::GetNextColor(const int CountFactor)
{
	static int Count = 0;
	Count += CountFactor;
	Count = OmniMath::CircularNum(ColorSet.size() - 1, Count);
	return ColorSet[Count];
}

FColor EOmniColor::GetNextColorByHue(const uint8 ColorSplitNumber, const uint8 CountFactor)
{
	// ColorSplitNumber단계로 색조 분할
	const float SliceInterval = 255.0f / static_cast<float>(ColorSplitNumber);

	static float Hue = 0;
	Hue += (SliceInterval * CountFactor);
	Hue = OmniMath::CircularNumF(255.0f, Hue);
	const uint8 Hue8 = static_cast<uint8>(FMath::RoundHalfToEven(Hue));

	// 배경과 대비를 위해 명도를 약간 낮춤.
	return FLinearColor::MakeFromHSV8(Hue8, 255, 255 * 0.8).ToFColor(true);
}
