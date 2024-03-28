// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <string>

DECLARE_LOG_CATEGORY_EXTERN(Enoch, Log, All);
#define ENLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define ENLOG_S(Verbosity) UE_LOG(Enoch, Verbosity, TEXT("%s"), *ENLOG_CALLINFO)
#define ENLOG(Verbosity, Format, ...) UE_LOG(Enoch, Verbosity, TEXT("%s%s"), *ENLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define ENCHECK(Expr, ...) { if(!(Expr)) {ENLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }
/**
 * convert String to FText.<br>
 * 한글 문자열을 FText로 변환하는데 사용.
 * FText::FromString(FString::Printf(TEXT(InString)))
 */
#define EN_STRING_TO_FTEXT(InString) FText::FromString(FString::Printf(TEXT(InString)))
std::wstring GetEnochDataPath();