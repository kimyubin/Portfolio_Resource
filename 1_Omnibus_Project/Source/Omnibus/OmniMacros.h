// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OmniMacros.generated.h"

//~=============================================================================
// 


//~=============================================================================
// 디버그 UI 매크로. 키워드(NAME)로 반복 작업 간소화.

/**
 * 디버그 UI용 선언부.
 * 델리게이트 핸들, Debug UI에 표기할 숫자를 저장하는 변수, 델리게이트를 선언합니다.
 * 변수를 증가시키는 함수를 선언 및 정의합니다.
 * @note UOmniPlayMainUI
 */
#define DEBUG_TEXT_MSG_DECL(NAME) \
private: \
	FDelegateHandle Post##NAME##Handle;\
	uint64 Debug##NAME##Count = 0;\
public:\
	static FOnPostPathFinding OnPost##NAME;\
	void Add##NAME##Count(const int32 InAddCount)\
	{\
		Debug##NAME##Count += InAddCount;\
		UpdateDebugText();\
	}


/**
 * NativeConstruct 델리게이트 등록
 * @note UOmniPlayMainUI::NativeConstruct
 */
#define DEBUG_TEXT_MSG_CONSTRUCT(NAME)    Post##NAME##Handle = OnPost##NAME.AddUObject(this, &UOmniPlayMainUI::Add##NAME##Count);

/**
 * NativeDestruct 델리게이트 해제
 * @note UOmniPlayMainUI::NativeDestruct
 */
#define DEBUG_TEXT_MSG_DESTRUCT(NAME)     OnPost##NAME.Remove(Post##NAME##Handle);

/**
 * 텍스트 내용 업데이트
 * @note UOmniPlayMainUI::UpdateDebugText
 */
#define DEBUG_TEXT_MSG_UPDATE(EXP, NAME)  DebugMsg += TEXT(EXP) + FString::FromInt(Debug##NAME##Count) + TEXT("\n");

/**
 * 델리게이트 정의
 * @note UOmniPlayMainUI
 */
#define DEBUG_TEXT_MSG_DELEGATE_DEFINE(NAME) UOmniPlayMainUI::FOnPostPathFinding UOmniPlayMainUI::OnPost##NAME;

/**
 * 
 */
UCLASS()
class OMNIBUS_API UOmniMacros : public UObject
{
	GENERATED_BODY()
	
};
