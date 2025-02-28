// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KuObject.generated.h"

class UKuGameInstance;
/**
 * 
 */
UCLASS()
class KKUMIGI_API UKuObject : public UObject
{
	GENERATED_BODY()
public:
	/** NewObject 이후, 런타임 초기화용 함수입니다. */
	virtual void Init();

protected:
	UKuGameInstance* GetKuGameInstance() const;
};
