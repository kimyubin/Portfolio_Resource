// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMGameInstance.h"

#include "EMPlayData.h"


void UEMGameInstance::Init()
{
	Super::Init();
}
void UEMGameInstance::BeginDestroy()
{
	Super::BeginDestroy();
}

AEMPlayData* UEMGameInstance::GetEMPlayData()
{
	if(EMPlayData==nullptr)
	{
		//공용 슬롯 이미지를 갖고 있는 액트 스폰.
		//_C는 C++코드 상속받은 블루프린트
		//UClass(오브젝트)는 BP_Name.BP_Name_C
		//Class이름은 그냥 BP_Name
		FName Path = TEXT("Class'/Game/GameManager/BP_EMPlayData.BP_EMPlayData_C'"); 
		UClass* UPlayData = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), NULL, *Path.ToString()));
		EMPlayData = GetWorld()->SpawnActor<AEMPlayData>(UPlayData,FVector::ZeroVector, FRotator::ZeroRotator);
	}
		
	return EMPlayData;
}

