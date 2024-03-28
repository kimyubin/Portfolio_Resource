// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CitadelStatics.generated.h"

/**
 * 블루프린트와 C++ 모두에서 호출할 수 있는 유틸리티 함수를 포함하는 static class
 * 생성자 생성하면 안됨.
 */
UCLASS()
class CITADELFOREVER_API UCitadelStatics : public UObject
{
	GENERATED_BODY()
public:
	/** 좌표평면 위에서 두 3차원 객체 사이의 거리. 수직 성분 없음. 직선거리가 아님. 평면 거리임*/
	static double CalPlaneDistance(FVector StartPoint, FVector EndPoint);

	/** round up. 유닛 단위로 한 칸씩 설치. 입력의 절댓값이 유닛 단위의 배수가 아니면, 절댓값 올림 */
	static double RoundUpForUnit(double InValue);
};
