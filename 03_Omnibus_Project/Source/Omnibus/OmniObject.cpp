// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniObject.h"

#include "OmnibusGameInstance.h"

void UOmniObject::Initialize(UOmnibusGameInstance* InOmniGameInstance)
{
	OmniGameInstance = InOmniGameInstance;
}

UOmnibusGameInstance* UOmniObject::GetOmniGameInstance() const
{
	if (UOmnibusGameInstance* OmniInstance = OmniGameInstance.Get())
		return OmniInstance;

	return nullptr;
}
