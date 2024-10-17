// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OmniObject.h"
#include "UObject/NoExportTypes.h"
#include "OmniOnlineSubsystem.generated.h"

class IOnlineSubsystem;
class IOnlineSession;
/**
 * 온라인 서브 시스템 레퍼 클래스입니다.
 */
UCLASS()
class OMNIBUS_API UOmniOnlineSubsystem : public UOmniObject
{
	GENERATED_BODY()

public:
	UOmniOnlineSubsystem() = default;

	virtual void Initialize(UOmnibusGameInstance* InOmniGameInstance) override;
private:
	IOnlineSubsystem* GetOnlineSubSystem() const;
};
