// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CitadelPlayData.h"
#include "CitadelBuilding.h"

// Sets default values
UCitadelPlayData::UCitadelPlayData()
{
	for (int i = 0; i < EnumToInt(EBuildingType::Size); ++i)
	{
		FBuildingTemplate temp;
		temp.BuildingType = static_cast<EBuildingType>(i);
		BuildingClassList.Add(temp);
	}
}

bool UCitadelPlayData::ChangeGold(int32 ResChanges)
{
	if (ResChanges < 0)
	{
		//돈 부족
		if (GoldsAvailable < static_cast<uint32>(ResChanges * -1))
			return false;
	}
	if(ResChanges > 0)
	{
		GoldsGathered += ResChanges;	
	}
	
	GoldsAvailable += ResChanges;	

	return true;
}

TSubclassOf<ACitadelBuilding> UCitadelPlayData::GetBuildingClass(uint8 BuildingTypeNum)
{
	if (BuildingTypeNum <= 0 && BuildingClassList.Num() <= BuildingTypeNum)
		return nullptr;
	return BuildingClassList[BuildingTypeNum].CitadelBuilding;
}

TSubclassOf<ACitadelBuilding> UCitadelPlayData::GetBuildingClass(EBuildingType BuildingType)
{
	return GetBuildingClass(EnumToInt(BuildingType));
}

UTexture2D* UCitadelPlayData::GetBuildingUIImage(uint8 BuildingTypeNum)
{
	if (BuildingTypeNum <= 0 && BuildingClassList.Num() <= BuildingTypeNum)
		return nullptr;
	return BuildingClassList[BuildingTypeNum].CitadelBuildingSlotImage;
}

UTexture2D* UCitadelPlayData::GetBuildingUIImage(EBuildingType BuildingType)
{
	return GetBuildingUIImage(EnumToInt(BuildingType));
}
