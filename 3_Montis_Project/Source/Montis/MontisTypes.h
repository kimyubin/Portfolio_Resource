// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MontisTypes.generated.h"

/** uint8 enum class를 uint8로 변환*/
#define EnumToInt(EN) static_cast<uint8>(EN)

/** 공용 상수*/
class MontisConstant
{
public:
	/** 단위 길이 당 월드 길이. 언리얼 1단위는 1cm와 동일. 프로젝트 단위 길이는 1m */
	// static constexpr double Unit_Range = 100.;
};

/**
 * 유틸리티 함수 호출용 static 구조체.
 */
USTRUCT()
struct MONTIS_API FMontisStatics
{
	GENERATED_BODY()
	
};