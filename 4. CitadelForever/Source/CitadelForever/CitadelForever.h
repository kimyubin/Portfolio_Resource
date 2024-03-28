// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(CitadelForever, Log, All);

//호출된 함수 이름과 라인 출력
#define CF_LOG_FUNC_LINE_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
//호출 함수 이름 및 라인 단순 출력
#define CF_LOG_FUNC() UE_LOG(CitadelForever, Warning, TEXT("%s"), *CF_LOG_FUNC_LINE_INFO)
/**호출 함수 이름, 라인 출력. string을 받아서 로그 출력. 서식 지정자 써서 사용가능.<br>
 * CF_LOG("A") or CF_LOG("A, %d")
 */
#define CF_LOG(Format, ...) UE_LOG(CitadelForever, Warning, TEXT("%s %s"), *CF_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
//string 형식 출력
//CF_LOG_STR(string)
#define CF_LOG_STR(Format) UE_LOG(CitadelForever, Warning, TEXT("%s"), *Format)
