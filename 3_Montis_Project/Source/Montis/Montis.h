// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Montis, Log, All);

//호출된 함수 이름과 라인 출력
#define MT_LOG_FUNC_LINE_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
//호출 함수 이름 및 라인 단순 출력
#define MT_LOG_FUNC() UE_LOG(Montis, Warning, TEXT("%s"), *MT_LOG_FUNC_LINE_INFO)

// 호출 함수 이름, 라인 출력. string을 받아서 로그 출력. 서식 지정자 써서 사용가능.<br>
// MT_LOG("A") or MT_LOG("A, %d",int i), MT_LOG(" %s",FVector *fv.ToString())
#define MT_LOG(Format, ...) UE_LOG(Montis, Warning, TEXT("%s %s"), *MT_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
//string 형식 출력
//MT_LOG_STR(string)
#define MT_LOG_STR(Format) UE_LOG(Montis, Warning, TEXT("%s"), *Format)
