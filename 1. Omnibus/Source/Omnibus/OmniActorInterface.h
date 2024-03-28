// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OmniActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOmniActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 공용 ID 함수 정의.
 */
class OMNIBUS_API IOmniActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** 아이디 생성 및 세팅. */
	virtual void   SetOmniID() = 0;
	virtual uint64 GetOmniID() = 0;
};
