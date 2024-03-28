// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolDetailSlot.h"

#include "SelectEnemyPoolUI.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEnemyPoolDetailSlot::SetDetailSlot( UMaterialInterface* Material, const int& FreeLancerCount )
{
	DetailSlotImg->SetBrushFromMaterial(Material);
	DetailSlotNum->SetText(FText::AsNumber(FreeLancerCount));
}
