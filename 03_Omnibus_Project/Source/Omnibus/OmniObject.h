// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OmniObject.generated.h"

class UOmnibusGameInstance;
/**
 * 프로젝트 최상위 UObject입니다.
 * 공통 초기화를 담당합니다.
 */
UCLASS()
class OMNIBUS_API UOmniObject : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * NewObject와 Initialize를 통합해 처리하는 함수입니다.
	 * 
	 * @tparam T UOmniObject의 자식 클래스.
	 */
	template <typename T>
		requires std::derived_from<T, UOmniObject>
	static T* NewInitObject(UOmnibusGameInstance* InGameInstance)
	{
		auto NewOmniObj = NewObject<T>(InGameInstance);
		NewOmniObj->Initialize(InGameInstance);
		return NewOmniObj;
	}

	/**
	 * NewObject와 Initialize를 통합해 처리하는 함수입니다.
	 * 
	 * @tparam T UOmniObject의 자식 클래스.
	 */
	template <typename T>
		requires std::derived_from<T, UOmniObject>
	static T* NewInitObject(UOmnibusGameInstance* InGameInstance, const UClass* InClass)
	{
		auto NewOmniObj = NewObject<T>(InGameInstance, InClass);
		NewOmniObj->Initialize(InGameInstance);
		return NewOmniObj;
	}

	virtual void Initialize(UOmnibusGameInstance* InOmniGameInstance);

	UOmnibusGameInstance* GetOmniGameInstance() const;

private:
	/** UObject에서 게임 인스턴스에 접근하기 위한 캐싱.*/
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UOmnibusGameInstance> OmniGameInstance;
};
