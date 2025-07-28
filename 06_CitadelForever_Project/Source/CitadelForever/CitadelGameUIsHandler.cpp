// Fill out your copyright notice in the Description page of Project Settings.


#include "CitadelGameUIsHandler.h"

#include "CitadelBuilding.h"


UCitadelGameUIsHandler::UCitadelGameUIsHandler()
{
	bSelectBuilding = false;
}

void UCitadelGameUIsHandler::SelectBuilding(ACitadelBuilding* InSelectedBuilding)
{
	if(SelectedBuilding.IsValid())
	{
		SelectedBuilding->SetSelectedBuildingMode(false);
	}
	SelectedBuilding = InSelectedBuilding;
	SelectedBuilding->SetSelectedBuildingMode(true);	
}
void UCitadelGameUIsHandler::DeselectBuilding()
{
	if(SelectedBuilding.IsValid())
	{
		SelectedBuilding->SetSelectedBuildingMode(false);
		SelectedBuilding.Reset();
	}
}
