// Fill out your copyright notice in the Description page of Project Settings.


#include "KuObject.h"

#include "KuGameInstance.h"

void UKuObject::Init()
{}

UKuGameInstance* UKuObject::GetKuGameInstance() const
{
	const UWorld* World = GetWorld();
	return World ? World->GetGameInstance<UKuGameInstance>() : nullptr;
}
