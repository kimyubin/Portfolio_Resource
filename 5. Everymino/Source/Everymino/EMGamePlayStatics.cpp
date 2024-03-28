// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMGamePlayStatics.h"
#include "EMUnitBrick.h"
#include "BrickTemplate.h"


AEMUnitBrick* UEMGamePlayStatics::SpawnUnit(int InX, int InY, UnitSkin SkinEnum, AActor* ParentActor)
{
	if(SkinEnum==UnitSkin::Empty||SkinEnum==UnitSkin::InvisibleWall)
		return nullptr;
	auto Res = ParentActor->GetWorld()->SpawnActor<AEMUnitBrick>(AEMUnitBrick::StaticClass());
	Res->AttachToActor(ParentActor, FAttachmentTransformRules::KeepRelativeTransform);
	Res->SetPositionAndSkin(InX, InY, SkinEnum);
	return Res;
}
AEMUnitBrick* UEMGamePlayStatics::SpawnUnit(FVector2I InLocation, UnitSkin SkinEnum, AActor* ParentActor)
{
	return SpawnUnit(InLocation.X, InLocation.Y, SkinEnum, ParentActor);
}
void UEMGamePlayStatics::DestroyUnit(AEMUnitBrick** InUnit)
{
	(*InUnit)->Destroy();
	*InUnit = nullptr;
}
EMInput UEMGamePlayStatics::GetInsideDirection(EDropStartDirection InStartDirection)
{
	EMInput Res = EMInput::None;
	
	switch (InStartDirection)
	{
	case EDropStartDirection::North:
		Res = EMInput::DownMove;
		break;
	case EDropStartDirection::East:
		Res = EMInput::LeftMove;
		break;
	case EDropStartDirection::South:
		Res = EMInput::UpMove;
		break;
	case EDropStartDirection::West:
		Res = EMInput::RightMove;
		break;
	default:
		break;
	}
	return Res;
}

EMInput UEMGamePlayStatics::GetOutsideDirection(EDropStartDirection InStartDirection)
{
	EMInput Res = EMInput::None;
	
	switch (InStartDirection)
	{
	case EDropStartDirection::North:
		Res = EMInput::UpMove;
		break;
	case EDropStartDirection::East:
		Res = EMInput::RightMove;
		break;
	case EDropStartDirection::South:
		Res = EMInput::DownMove;
		break;
	case EDropStartDirection::West:
		Res = EMInput::LeftMove;
		break;
	default:
		break;
	}
	return Res;
}
