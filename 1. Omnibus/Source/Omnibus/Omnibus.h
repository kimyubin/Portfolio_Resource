// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Omnibus, Log, All);

/** 변수명 출력 */
#define GET_VAR_NAME(V) #V

/** 호출된 함수 이름과 라인 출력 */
#define OB_LOG_FUNC_LINE_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

/** 호출 함수 이름 및 라인 단순 출력 */
#define OB_LOG_FUNC() UE_LOG(Omnibus, Warning, TEXT("%s"), *OB_LOG_FUNC_LINE_INFO)

/**
 * 호출 함수 이름, 라인 출력. string을 받아서 로그 출력. 서식 지정자 써서 사용가능.<br>
 * OB_LOG("A") or OB_LOG("A, %d",int i), OB_LOG(" %s",FVector *fv.ToString())
 */
#define OB_LOG(Format, ...) UE_LOG(Omnibus, Warning, TEXT("%s %s"), *OB_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

#define OB_ERROR(Format, ...) UE_LOG(Omnibus, Error, TEXT("%s %s"), *OB_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * string 형식 출력 <br>
 * OB_LOG_STR(string)
 */
#define OB_LOG_STR(Format) UE_LOG(Omnibus, Warning, TEXT("%s %s"), *OB_LOG_FUNC_LINE_INFO, *FString(Format))

/** 스크린 출력 */
#define OB_DEBUG_MSG(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__), true, FVector2D::UnitVector * 0.75);

struct FOBCheck
{
 static int Number;
};
/** 순서 확인용 */
#define OB_ORDER_CHECK() OB_LOG("%s / check num: %d", *GetName(), ++FOBCheck::Number)

/** 시간 측정 */
#define OB_TIME_START() std::chrono::system_clock::time_point TimeCheckStart = std::chrono::system_clock::now();
#define OB_TIME_CHECK(Format) OB_LOG("%s %lld", Format, std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - TimeCheckStart).count()) \
 TimeCheckStart = std::chrono::system_clock::now();
#define OB_TIME_END() OB_LOG("%s %lld", "TimeCheckEnd", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - TimeCheckStart).count())