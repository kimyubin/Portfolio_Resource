// Fill out your copyright notice in the Description page of Project Settings.

#include "OmniActorInterface.h"

#include "OmnibusTypes.h"

#include "UtlLog.h"
#include "UTLStatics.h"

#include <string>

// Add default functionality here for any IOmniActorInterface functions that are not pure virtual.

std::atomic<uint64> UOmniID::ID_TodayCountAtomic = 0;


uint64 UOmniID::GenerateID_Number(const AActor* InActor)
{
	return GenerateID_NumberByHash(InActor);
	// return GenerateID_NumberByDate();
}

uint64 UOmniID::GenerateID_NumberByDate()
{
	++ID_TodayCountAtomic;

	const FDateTime NowT   = FDateTime::Now();
	const uint64 T_Year    = UtlBit::ExtractFillBit(NowT.GetYear() - 1900, 0, 10);
	const uint64 T_Month   = UtlBit::ExtractFillBit(NowT.GetMonth(), 10, 4);
	const uint64 T_Day     = UtlBit::ExtractFillBit(NowT.GetDay(), 14, 5);
	const uint64 T_RunSec  = UtlBit::ExtractFillBit(FUtlTime::GetLevelStartTime_Sec(), 19, 17);
	const uint64 ID_Number = T_Year | T_Month | T_Day | T_RunSec | ID_TodayCountAtomic;

	UT_LOG_STR(*GetID_DateToString(ID_Number))

	return ID_Number;
}

FString UOmniID::GetID_DateToString(const uint64 InID)
{
	const uint64 T_Year   = UtlBit::GetSubBit(InID, 0, 10);
	const uint64 T_Month  = UtlBit::GetSubBit(InID, 10, 4);
	const uint64 T_Day    = UtlBit::GetSubBit(InID, 14, 5);
	const uint64 T_RunSec = UtlBit::GetSubBit(InID, 19, 17);
	const uint64 T_Count  = UtlBit::GetSubBit(InID, 36, 28);

	return FString::FromInt(T_Year + 1900) + "-"
			+ FString::FromInt(T_Month) + "-"
			+ FString::FromInt(T_Day) + " "
			+ FString::FromInt(T_RunSec / 3600) + ":"
			+ FString::FromInt(T_RunSec % 3600 / 60) + ":"
			+ FString::FromInt(T_RunSec % 3600 % 60) + " "
			+ FString::FromInt(T_Count) + "  "
			+ FString::FromInt(InID);
}

uint64 UOmniID::GenerateID_NumberByPos(const AActor* InActor)
{
	if (IsValid(InActor) == false)
		return 0;

	const UWorld* ActorWorld = InActor->GetWorld();
	if (IsValid(ActorWorld) == false)
		return 0;

	//x,y위치와 레벨로 ID 생성
	//x,y위치 24비트. 레벨 태그는 16비트.
	const int64  InActorLocationX = FMath::RoundHalfToEven(InActor->GetActorLocation().X / FOmniConst::Unit_Length);
	const int64  InActorLocationY = FMath::RoundHalfToEven(InActor->GetActorLocation().Y / FOmniConst::Unit_Length);
	const uint64 LevelTag         = InActor->GetLevel()->GetUniqueID();

	uint64 ID_Number(0);
	ID_Number |= UtlBit::ExtractFillBit(InActorLocationX, 0, 24);
	ID_Number |= UtlBit::ExtractFillBit(InActorLocationY, 24, 24);
	ID_Number |= UtlBit::ExtractFillBit(LevelTag, 48, 16);

	UT_LOG_STR(*GetID_InfoToString(ID_Number))

	return ID_Number;
}

uint64 UOmniID::GenerateID_NumberByHash(const AActor* InActor)
{
	if (IsValid(InActor) == false)
	{
		UT_LOG("invalid_Actor")
		return 0;
	}

	const int64 InActorLocationX = FMath::RoundHalfToEven(InActor->GetActorLocation().X / FOmniConst::Unit_Length);
	const int64 InActorLocationY = FMath::RoundHalfToEven(InActor->GetActorLocation().Y / FOmniConst::Unit_Length);
	const FString HashSource     = FString::Printf(TEXT("%s_%lld_%lld"), *InActor->GetName(), InActorLocationX, InActorLocationY);

	constexpr std::hash<std::string> Hasher;
	return Hasher(TCHAR_TO_ANSI(*HashSource));
}

FString UOmniID::GetID_InfoToString(const uint64 InID)
{
	const int x         = UtlBit::GetSignedBitRange(InID, 0, 24) * FOmniConst::Unit_Length;
	const int y         = UtlBit::GetSignedBitRange(InID, 24, 24) * FOmniConst::Unit_Length;
	const int ActorType = UtlBit::GetSignedBitRange(InID, 48, 16);
	
	return "X: " + FString::FromInt(x) + ", Y: " + FString::FromInt(y) + ", Level Type: " + FString::FromInt(ActorType);
}