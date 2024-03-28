// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Omnibus.h"
#include "UObject/NoExportTypes.h"
#include "OmnibusUtilities.generated.h"

////////////////////////////////////////////////
// 게임플레이 유틸리티 정적 클래스 및 함수 모음
////////////////////////////////////////////////

/**
 * 개체 유효성 테스트 및 로그 출력.
 * 로그가 필요한 경우에만 사용해야함.
 * 유효하지 않을 경우, 로그 출력 및 false 반환.
 * @param UObjPtr : 테스트할 개체
 * @return 객체가 사용가능하면 true 반환. null, pending kill, 가비지 수집 대상인 경우 false 반환.
 * @see IsOmniValidLog
 */
#define OB_IS_VALID(UObjPtr) IsOmniValidLog(UObjPtr, GET_VAR_NAME(UObjPtr), OB_LOG_FUNC_LINE_INFO, this->GetName())

/**
 * 개체 유효성 테스트 및 로그 출력
 * @param    Test           테스트할 개체
 * @param    InVarName      테스트할 개체의 변수명
 * @param    InLogFuncInfo  오류가 난 함수 위치 정보.
 * @param    OwnerObjName   오류가 난 개체의 이름.
 * @return   객체가 사용가능하면 true 반환. null, pending kill, 가비지 수집 대상이 아닌 경우.
 * @see OB_IS_VALID
 */
FORCEINLINE bool IsOmniValidLog(const UObject* Test, const FString& InVarName, const FString& InLogFuncInfo, const FString& OwnerObjName)
{	
	if (IsValid(Test))
		return true;

	UE_LOG(Omnibus, Warning, TEXT("%s %s is not valid. (%s)"), *InLogFuncInfo, *InVarName, *OwnerObjName)
	
	return false;
}

/** 객체 ID 생성용 */
UCLASS()
class OMNIBUS_API UOmniID : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * 상위 19bit : 날짜 정보. 10bit(연) : 1900 ~ 2924, 4bit(월) : 1~12, 5bit(일) : 1~31
	 * 중위 17bit : 실행 시간 초
	 * 하위 28bit : 일일 생성 순번
	 */
	static uint64 GenerateID_Number();

	static std::string GetID_DateToString(const uint64 InID);

	/** 액터가 스폰된 위치와 레벨 기반으로 ID 생성. */
	static uint64 GenerateID_NumberByPos(const AActor* InActor);
	static std::string GetID_InfoToString(const uint64 InID);

private:
	static std::atomic<uint64> ID_TodayCountAtomic;
};

namespace OmniTool
{
	/**
	 * 문자열과 인덱스를 합쳐서 FName으로 반환.
	 * @return "InNameString_InIdx" 
	 */
	FName ConcatStrInt(const std::string& InNameString, const int32 InIdx);
}

/**
 * 언리얼 엔진 컨테이너 커스텀 함수 모음.
 */
namespace OmniContainer
{
	/////////////
	// TMap 관련

	/**
	 * 키와 값을 TMap에 추가. 키값이 이미 있으면, 해당 키에 대한 값 갱신
	 * TMap 멤버 할당 및 값 갱신 함수.
	 *
	 * @param InMap : 멤버를 추가할 대상 TMap
	 * @param InKey : 키
	 * @param InValue : 값
	 */
	template <typename InKeyType, typename InValueType, typename SetAllocator, typename KeyFuncs>
	FORCEINLINE void TMap_Emplace(TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>& InMap, const InKeyType& InKey, const InValueType& InValue)
	{
		InValueType* ValuePtr = InMap.Find(InKey);
		if (ValuePtr == nullptr)
			InMap.Emplace(InKey, InValue);
		else
			*ValuePtr = InValue;
	}

	/**
	 * TMap의 지정된 키와 연결된 값을 찾아서 반환. 값이 없거나, 유효하지 않을 경우 nullptr 반환
	 * 
	 * @param InMap 찾을 대상 TMap
	 * @param InKey 찾고자 하는 키
	 * @return 찾아낸 값. 유효하지 않거나 값이 없으면 nullptr 반환
	 */
	template <typename InKeyType, typename InValueType, typename SetAllocator, typename KeyFuncs>
	FORCEINLINE InValueType TMap_Find(TMap<InKeyType, InValueType, SetAllocator, KeyFuncs>& InMap, const InKeyType& InKey)
	{
		InValueType* ValuePtr = InMap.Find(InKey);
		if (ValuePtr == nullptr)
			return nullptr;
		else
			return IsValid(*ValuePtr) ? *ValuePtr : nullptr;
		
	}
}

/** 시간 관련 함수 및 변수를 취급 */
struct FOmniTime
{
	static void   SetLevelStartTime_Sec();
	static uint64 GetLevelStartTime_Sec();
	static uint64 GetNowTime_Sec();
	
private:
	/** 게임 시작 시각. 초로 기록 */
	static uint64 LevelStartTime_Sec;
};

/** 비트연산 */
namespace OmniBit
{
	/**
	 * 입력 비트값의 하위 BitCount만큼 추출 후, 시작 위치부터 지정된 개수(count)만큼 채워서 반환. 지정된 비트수 외의 공간은 0으로 채움.
	 * @param InBitVal : 비트값
	 * @param InStartBitOffset : 반환값에서 시작할 지점. 좌측부터, 0~63
	 * @param InBitCount : 사용할(채울) 비트 개수, 1~64 
	 */
	uint64 ExtractFillBit(const uint64 InBitVal, const uint64 InStartBitOffset, const uint64 InBitCount);

	/**
	 * 지정된 범위의 비트를 추출함.(Unsigned)
	 * 입력 비트값을 시작 지점부터 지정된 개수만큼 뽑아, 하위 비트에 채워 반환.
	 * 부호 없음. 
	 * @param InBitValue : 비트값
	 * @param InStartBitOffset : 시작 지점. 좌측부터, 0~63
	 * @param InBitCount : 가져올 비트 개수, 1~64
	 */
	uint64 GetSubBit(const uint64 InBitValue, const uint64 InStartBitOffset, const uint64 InBitCount);

	/**
	 * 지정된 범위의 비트를 추출함.(Signed)
	 * 입력 비트값을 시작 지점부터 지정된 개수만큼 뽑아, 하위 비트에 채워 반환. 첫 비트가 1인 경우 음수로 인식해서 상위 비트를 0 대신 1로 채움
	 * @param InBitValue : 비트값
	 * @param InStartBit : 시작 지점. 좌측부터, 0~63
	 * @param InBitCount : 가져올 비트 개수, 1~64
	 */
	int64 GetSignedBitRange(const uint64 InBitValue, const uint64 InStartBit, const uint64 InBitCount);
}

/** 수식 유틸리티 */
namespace OmniMath
{
	/**
	 * 입력된 숫자를 주어진 범위[0, Max] 내에서 순환시켜 반환.
	 * 음수, 양수 양방향 변환
	 * @param InMax 범위 최대값(범위에 포함됨)
	 * @param InNum 입력값.
	 * @return 범위[0, Max] 내 순환한 값
	 */
	uint32 CircularNum(const uint32 InMax, const int64 InNum);
	
	/**
	 * offset 만큼 떨어진 곳에 있는 대상의 절대 Transform 계산
	 * @param InPos : 자신의 위치
	 * @param InDirection : 상대 방향
	 * @param InOffset : 상대 위치 거리
	 * @return Offset 적용 후 Transform
	 */
	FTransform GetTransformAddOffset(const FVector& InPos, const FVector& InDirection, const double InOffset);

	//////////////////
	//벡터, 차원 관련

	FVector2D MakeFVector2D(const FVector& InVector);
	FVector   MakeFVector(const FVector2D& InVector, const double InZ = 0.0);
	
	/**
	 * 경계구역 체크. 좌상, 우하 순서 입력 권장.
	 * @param InVector2D : 체크할 벡터
	 * @param A : 경계가 되는 사각형의 한쪽 모서리. B의 대각 
	 * @param B : 경계가 되는 사각형의 한쪽 모서리. A의 대각
	 */
	FVector2D ClampVector2D(const FVector2D& InVector2D, const FVector2D& A, const FVector2D& B);
	
	/**
	 * 경계구역 체크. 좌상, 우하 순서 입력 권장. z축은 체크하지 않고, 입력된 z축을 그대로 반환.
	 * @param InVector : 체크할 벡터
	 * @param A : 경계가 되는 사각형의 한쪽 모서리. B의 대각 
	 * @param B : 경계가 되는 사각형의 한쪽 모서리. A의 대각
	 * @return : z축 제외 후 경계 체크된 벡터
	 */
	FVector ClampVector(const FVector& InVector, const FVector& A, const FVector& B);

	/**
	 * 입력된 축을 기준으로 FRotator를 회전 후 반환.
	 * @param InRotator 회전할 FRotator
	 * @param InAngle 회전 각도.
	 * @param InAxis 회전 축. 내부에서 정규화해서 처리
	 * @return 회전된 FRotator
	 */
	FRotator RotateAxis(const FRotator& InRotator, const double InAngle, const FVector& InAxis);

	/**
	 * X축을 기준으로 회전
	 * @param InRotator 회전할 FRotator
	 * @param InAngle 회전량. 단위는 도.
	 * @return 회전 후 결과 FRotator
	 */
	FRotator RotateAxisX(const FRotator& InRotator, const double InAngle);

	/**
	 * Y축을 기준으로 회전
	 * @param InRotator 회전할 FRotator
	 * @param InAngle 회전량. 단위는 도.
	 * @return 회전 후 결과 FRotator
	 */
	FRotator RotateAxisY(const FRotator& InRotator, const double InAngle);

	/**
	 * Z축을 기준으로 회전
	 * @param InRotator 회전할 FRotator
	 * @param InAngle 회전량. 단위는 도.
	 * @return 회전 후 결과 FRotator
	 */
	FRotator RotateAxisZ(const FRotator& InRotator, const double InAngle);

	/**
	 * 박스의 폭(y)을 반환
	 * @param InBox 계산할 FBox
	 * @return 박스의 폭
	 */
	double GetBoxWidth(const FBox& InBox);
	
	/////////////////
	//수치 관련

	/**
	 * 반올림. 0.5는 가장 가까운 짝수값이 됨.<br>
	 * 양쪽의 정수가 동일한 거리에 있는 경우(==0.5), 가장 가까운 짝수 값 반환.<br>
	 * 1.5-> 2, 0.5->0, -1.5->2, -0.5->0
	 * @param InDouble : 반올림할 부동 소수점 숫자
	 * @param InUnitValue : 반올림에 사용할 단위. 50을 입력하면 50단위로 반올림 처리.
	 * */
	double RoundHalfToEvenByUnitValue(const double InDouble, const double InUnitValue = 1.0);

	/**
	 * InUnitValue단위로 스냅되게끔 벡터 위치를 조정 .<br>
	 * 양쪽의 정수가 동일한 거리에 있는 경우(==0.5), 가장 가까운 짝수 값 반환.<br>
	 * 1.5-> 2, 0.5->0, -1.5->2, -0.5->0
	 * @param InVector : 반올림할 FVector
	 * @param InUnitValue : 반올림에 사용할 단위. 50을 입력하면 50단위로 반올림 처리.
	 * */
	FVector RoundHalfToEvenVector(const FVector& InVector, const double InUnitValue = 1.0);

}

/**
 * 테스트용 3D 텍스트 Render
 * ATextRenderActor를 정해진 시간동안만 월드에 스폰시킴.
 */
#define TIME_LIMIT_TEXT_RENDER( SpawnPos, LifeSpan, SettingText, WorldSize )\
{\
	auto textRender = GetWorld()->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), SpawnPos,FRotator(90.0,0.0,0.0));\
	textRender->SetLifeSpan(LifeSpan);\
	textRender->GetTextRender()->SetText(SettingText);\
	textRender->GetTextRender()->SetWorldSize(WorldSize);\
	textRender->GetTextRender()->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);\
	textRender->GetTextRender()->SetTextRenderColor(FColor::Red);\
}