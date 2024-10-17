// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Utilibrary.h"


/** 변수명 출력 */
#define GET_VAR_NAME(V) #V

/** 호출된 함수 이름과 라인 정보 반환 */
#define UT_LOG_FUNC_LINE_INFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))

/** 호출 함수 이름 및 라인 단순 출력 */
#define UT_LOG_FUNC() UE_LOG(UTL, Warning, TEXT("%s"), *UT_LOG_FUNC_LINE_INFO)


/**
 * 호출 함수 이름, 라인 출력. string을 받아서 로그 출력. 서식 지정자 써서 사용가능.<br>
 * UT_LOG("A") or UT_LOG("A, %d",int i), UT_LOG(" %s",FVector *fv.ToString())
 */
#define UT_LOG(Format, ...) UE_LOG(UTL, Warning, TEXT("%s %s"), *UT_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

#define UT_ERROR(Format, ...) UE_LOG(UTL, Error, TEXT("%s %s"), *UT_LOG_FUNC_LINE_INFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

/** 조건이 false이면 로그만 남기는 가벼운 검사입니다. */
#define UT_LIGHT_CHECK(cond, Format, ...) if ((cond) == false) { UT_LOG(Format, ##__VA_ARGS__) }

/**
 * string 형식 출력 <br>
 * UT_LOG_STR(string)
 */
#define UT_LOG_STR(Format) UE_LOG(UTL, Warning, TEXT("%s %s"), *UT_LOG_FUNC_LINE_INFO, *FString(Format))

/** 스크린 출력 */
#define UT_DEBUG_MSG(LifeTime, Format, ...)\
UT_IF (GEngine != nullptr)\
{\
    UT_LOG("%s", *OnlineSubsystemPtr->GetSubsystemName().ToString())\
    GEngine->AddOnScreenDebugMessage(INDEX_NONE, LifeTime, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__), true, FVector2D::UnitVector);\
}
//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__), true, FVector2D::UnitVector * 0.75);

// #define UT_DEBUG_MSG(Format, ...) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__), true, FVector2D::UnitVector * 0.75);


struct FUTCheck
{
	static int Number;
};
/** 순서 확인용 */
#define UT_ORDER_CHECK() UT_LOG("%s / check num: %d", *GetName(), FUTCheck::Number++)

