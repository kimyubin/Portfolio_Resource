// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Everymino, Log, All);

//호출된 함수 이름과 라인 출력
#define EM_LOG_FUNC_LINE_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
//호출 함수 이름 및 라인 단순 출력
#define EM_LOG_FUNC() UE_LOG(Everymino, Warning, TEXT("%s"), *EM_LOG_FUNC_LINE_INFO)
/**호출 함수 이름, 라인 출력. string을 받아서 로그 출력. 서식 지정자 써서 사용가능.<br>
 * EM_LOG("A") or EM_LOG("A, %d")
 */
#define EM_LOG(Format, ...) UE_LOG(Everymino, Warning, TEXT("%s %s"), *EM_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
//string 형식 출력
//EM_LOG_STR(string)
#define EM_LOG_STR(Format) UE_LOG(Everymino, Warning, TEXT("%s"), *Format)
