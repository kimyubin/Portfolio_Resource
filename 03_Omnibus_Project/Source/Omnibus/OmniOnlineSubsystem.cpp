// Fill out your copyright notice in the Description page of Project Settings.


#include "OmniOnlineSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

#include "UtlLog.h"
#include "UTLStatics.h"


void UOmniOnlineSubsystem::Initialize(UOmnibusGameInstance* InOmniGameInstance)
{
	Super::Initialize(InOmniGameInstance);

	if (IOnlineSubsystem* OnlineSubsystemPtr = Online::GetSubsystem(GetWorld()))
	{
		// OnlineSubsystem 이름 출력
		UT_DEBUG_MSG(20.f, "%s", *OnlineSubsystemPtr->GetSubsystemName().ToString())
	}
}

IOnlineSubsystem* UOmniOnlineSubsystem::GetOnlineSubSystem() const
{
	return Online::GetSubsystem(GetWorld());
}
