// Fill out your copyright notice in the Description page of Project Settings.

#include "OmnibusUtilities.h"

#include <iomanip>
#include <sstream>
#include <string>

#include "OmnibusTypes.h"
#include "Omnibus.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Notifications/SNotificationList.h"

std::atomic<uint64> UOmniID::ID_TodayCountAtomic = 0;
uint64 FOmniTime::LevelStartTime_Sec = 0;

void OmniMsg::Dialog(const FString& InMsg)
{
	FMessageDialog::Debugf(FText::FromString(InMsg));
}

void OmniMsg::ToastMsg(const FString& InMsg)
{
	const FText MsgTxt = FText::FromString(InMsg);
	const FNotificationInfo ErrorNotification(MsgTxt);
	FSlateNotificationManager::Get().AddNotification(ErrorNotification);
}

uint64 UOmniID::GenerateID_Number(const AActor* InActor)
{
	return GenerateID_NumberByHash(InActor);
	// return GenerateID_NumberByDate();
}

uint64 UOmniID::GenerateID_NumberByDate()
{
	++ID_TodayCountAtomic;

	const FDateTime NowT   = FDateTime::Now();
	const uint64 T_Year    = OmniBit::ExtractFillBit(NowT.GetYear() - 1900, 0, 10);
	const uint64 T_Month   = OmniBit::ExtractFillBit(NowT.GetMonth(), 10, 4);
	const uint64 T_Day     = OmniBit::ExtractFillBit(NowT.GetDay(), 14, 5);
	const uint64 T_RunSec  = OmniBit::ExtractFillBit(FOmniTime::GetLevelStartTime_Sec(), 19, 17);
	const uint64 ID_Number = T_Year | T_Month | T_Day | T_RunSec | ID_TodayCountAtomic;

	OB_LOG_STR(GetID_DateToString(ID_Number).c_str())

	return ID_Number;
}

std::string UOmniID::GetID_DateToString(const uint64 InID)
{
	const uint64 T_Year   = OmniBit::GetSubBit(InID, 0, 10);
	const uint64 T_Month  = OmniBit::GetSubBit(InID, 10, 4);
	const uint64 T_Day    = OmniBit::GetSubBit(InID, 14, 5);
	const uint64 T_RunSec = OmniBit::GetSubBit(InID, 19, 17);
	const uint64 T_Count  = OmniBit::GetSubBit(InID, 36, 28);

	return std::to_string(T_Year + 1900) + "-"
			+ std::to_string(T_Month) + "-"
			+ std::to_string(T_Day) + " "
			+ std::to_string(T_RunSec / 3600) + ":"
			+ std::to_string(T_RunSec % 3600 / 60) + ":"
			+ std::to_string(T_RunSec % 3600 % 60) + " "
			+ std::to_string(T_Count) + "  "
			+ std::to_string(InID);
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
	ID_Number |= OmniBit::ExtractFillBit(InActorLocationX, 0, 24);
	ID_Number |= OmniBit::ExtractFillBit(InActorLocationY, 24, 24);
	ID_Number |= OmniBit::ExtractFillBit(LevelTag, 48, 16);

	OB_LOG_STR(GetID_InfoToString(ID_Number).c_str())

	return ID_Number;
}

uint64 UOmniID::GenerateID_NumberByHash(const AActor* InActor)
{
	if (IsValid(InActor) == false)
	{
		OB_LOG("invalid_Actor")
		return 0;
	}

	const int64 InActorLocationX = FMath::RoundHalfToEven(InActor->GetActorLocation().X / FOmniConst::Unit_Length);
	const int64 InActorLocationY = FMath::RoundHalfToEven(InActor->GetActorLocation().Y / FOmniConst::Unit_Length);
	FString HashSource = FString::Printf(TEXT("%s_%lld_%lld"), *InActor->GetName(), InActorLocationX, InActorLocationY);

	constexpr std::hash<std::string> Hasher;
	return Hasher(TCHAR_TO_ANSI(*HashSource));
}

std::string UOmniID::GetID_InfoToString(const uint64 InID)
{
	const int x         = OmniBit::GetSignedBitRange(InID, 0, 24) * FOmniConst::Unit_Length;;
	const int y         = OmniBit::GetSignedBitRange(InID, 24, 24) * FOmniConst::Unit_Length;;
	const int ActorType = OmniBit::GetSignedBitRange(InID, 48, 16);
	
	return "X: " + std::to_string(x) + ", Y: " + std::to_string(y) + ", Level Type: " + std::to_string(ActorType);
}

FName OmniStr::ConcatStrInt(const std::string& InNameString, const int32 InIdx)
{
	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << InIdx;
	const std::string TwoDigitIdxStr = oss.str();

	return FName((InNameString + "_" + TwoDigitIdxStr).c_str());
}

FString OmniStr::EnumToString(const ECityBlockType InBlockType)
{
	switch (InBlockType)
	{
		case ECityBlockType::None:
			return "None";
		case ECityBlockType::Redevelopment:
			return "Redevelopment";
		case ECityBlockType::Residence:
			return "Residence";
		case ECityBlockType::Commerce:
			return "Commerce";
		case ECityBlockType::Industry:
			return "Industry";
		case ECityBlockType::Tour:
			return "Tour";
		case ECityBlockType::GreenSpace:
			return "GreenSpace";
		case ECityBlockType::Size:
			return "Size";
	}
	return "";
}

uint32 OmniMath::CircularNum(const uint32 InMax, const int64 InNum)
{
	// InMax를 범위에 포함하기 위해 1 더함.
	const int64 MaxOutLine = static_cast<int64>(InMax) + 1;
	if (InNum < 0)
		return (InNum % MaxOutLine) + MaxOutLine;
	else if (InNum <= InMax)
		return InNum;
		
	return InNum % MaxOutLine;
}

double OmniMath::CircularNumF(const double InMax, const double InNum)
{
	// InMax를 범위에 포함하기 위해 1 더함.
	const double MaxOutLine = InMax + 1.0;
	if (InNum < 0)
		return fmod(InNum, MaxOutLine) + MaxOutLine;
	else if (InNum <= InMax)
		return InNum;
	
	return fmod(InNum, MaxOutLine);
}

FTransform OmniMath::GetTransformAddOffset(const FVector& InPos, const FVector& InDirection, const double InOffset)
{
	const FVector ResVector = InPos + InDirection * InOffset;
	return FTransform(InDirection.Rotation(), ResVector);
}

void FOmniTime::SetLevelStartTime_Sec()
{
	LevelStartTime_Sec = GetNowTime_Sec();
}

uint64 FOmniTime::GetLevelStartTime_Sec()
{
	if (LevelStartTime_Sec == 0)
		SetLevelStartTime_Sec();

	return LevelStartTime_Sec;
}

uint64 FOmniTime::GetNowTime_Sec()
{
	const FDateTime NowTime = FDateTime::Now();
	return static_cast<uint64>(NowTime.GetSecond() + NowTime.GetMinute() * 60 + NowTime.GetHour() * 3600);
}

bool FOmniStatics::GetOverlapComps(UPrimitiveComponent* InTestComp, UClass* InComponentClassFilter, TArray<UPrimitiveComponent*>& OutOverlapComps)
{
	const EObjectTypeQuery CollObjectType = UEngineTypes::ConvertToObjectType(InTestComp->GetCollisionObjectType());

	return UKismetSystemLibrary::ComponentOverlapComponents(InTestComp
	                                                      , InTestComp->GetComponentTransform()
	                                                      , {CollObjectType}
	                                                      , InComponentClassFilter
	                                                      , TArray<AActor*>()
	                                                      , OutOverlapComps);
}

bool FOmniStatics::GetOverlapActors(UPrimitiveComponent* InTestComp, UClass* InActorClassFilter, TArray<AActor*>& OutOverlapActors)
{
	EObjectTypeQuery CollObjectTypes = UEngineTypes::ConvertToObjectType(InTestComp->GetCollisionObjectType());

	return UKismetSystemLibrary::ComponentOverlapActors(InTestComp
	                                                  , InTestComp->GetComponentTransform()
	                                                  , {CollObjectTypes}
	                                                  , InActorClassFilter
	                                                  , TArray<AActor*>()
	                                                  , OutOverlapActors);
}

FAssetData FOmniStatics::GetAssetByObjectPath(const FSoftObjectPath& ObjectPath, bool bIncludeOnlyOnDiskAssets, bool bSkipARFilteredAssets)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	return AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath, bIncludeOnlyOnDiskAssets, bSkipARFilteredAssets);
}

bool FOmniStatics::IsAssetExists(const FSoftObjectPath& ObjectPath)
{
	return GetAssetByObjectPath(ObjectPath).IsValid();
}

bool FOmniStatics::CreateUniqueAssetName(const FString& ObjectPath, FString& OutAssetPathName)
{
	// 이미 해당 경로에 애셋이 있는지 확인.
	bool bAssetExists  = true;
	FString NewName    = ObjectPath;

	constexpr int Loop = 1000;
	for (int count = 0; count < Loop; ++count)
	{
		bAssetExists = IsAssetExists(NewName);
		if (bAssetExists == false)
			break;

		NewName = ObjectPath + "_" + FString::FromInt(count);
	}

	// 중복된 이름. 이미 에셋 이름이 존재합니다.
	if (bAssetExists)
	{
		OmniMsg::ToastMsg("Duplicate name. The asset name already exists.");
		return false;
	}

	OutAssetPathName = NewName;
	return true;
}

uint64 OmniBit::ExtractFillBit(const uint64 InBitVal, const uint64 InStartBitOffset, const uint64 InBitCount)
{
	if (InStartBitOffset > 63 || InBitCount > 64 || InStartBitOffset + InBitCount > 64)
	{
		OB_LOG_STR("Bit Range Over")
		return 0;
	}
	
	//하위 비트를 BitCount만큼 추출
	int64 res = InBitVal & ((static_cast<uint64>(1) << InBitCount) - 1);
	//추출한 비트를 시작 지점까지 이동
	res = res << (63 - (InStartBitOffset + InBitCount - 1));

	return res;
}

uint64 OmniBit::GetSubBit(const uint64 InBitValue, const uint64 InStartBitOffset, const uint64 InBitCount)
{
	if (InStartBitOffset > 63 || InBitCount > 64 || (InStartBitOffset + InBitCount) > 64)
	{
		OB_LOG_STR("Bit Range Over")
		return 0;
	}

	//InStartBitOffset번째 비트부터 InBitCount만큼 마스킹
	int64 res = InBitValue >> (63 - (InStartBitOffset + InBitCount - 1));
	res &= (static_cast<uint64>(1) << InBitCount) - 1;

	return res;
}

int64 OmniBit::GetSignedBitRange(const uint64 InBitValue, const uint64 InStartBit, const uint64 InBitCount)
{
	if (InStartBit > 63 || InBitCount > 64 || InStartBit + InBitCount > 64)
	{
		OB_LOG_STR("Bit Range Over")
		return 0;
	}

	//InStartBit번째 비트부터 InBitCount만큼 마스킹
	int64 res = InBitValue >> (63 - (InStartBit + InBitCount - 1));
	res &= (static_cast<uint64>(1) << InBitCount) - 1;

	// 음수일 경우, 상위 비트 1로 채움
	if ((res >> (InBitCount - 1)) == 1)
	{
		const uint64 All1 = ~static_cast<uint64>(0) << InBitCount;
		res = res | All1;
	}

	return res;
}

FVector2D OmniMath::MakeFVector2D(const FVector& InVector)
{
	return FVector2D(InVector.X, InVector.Y);
}

FVector OmniMath::MakeFVector(const FVector2D& InVector, const double InZ/*= 0.0*/)
{
	return FVector(InVector.X, InVector.Y, InZ);
}

FVector2D OmniMath::ClampVector2D(const FVector2D& InVector2D, const FVector2D& A, const FVector2D& B)
{
	const FVector2D InMin = FVector2D(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y));
	const FVector2D InMax = FVector2D(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y));

	return FVector2D(FMath::Clamp(InVector2D.X, InMin.X, InMax.X)
	               , FMath::Clamp(InVector2D.Y, InMin.Y, InMax.Y));
}

FVector OmniMath::ClampVector(const FVector& InVector, const FVector& A, const FVector& B)
{
	const FVector InMin = FVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), InVector.Z);
	const FVector InMax = FVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), InVector.Z);

	return FVector(FMath::Clamp(InVector.X, InMin.X, InMax.X)
	             , FMath::Clamp(InVector.Y, InMin.Y, InMax.Y)
	             , InVector.Z);
}

FRotator OmniMath::RotateAxis(const FRotator& InRotator, const double InAngle, const FVector& InAxis)
{
	//입력 축을 기준으로 입력 각도만큼 회전한 쿼터니언을 구함.	
	const FQuat InQuat = FQuat(InAxis.GetSafeNormal(), FMath::DegreesToRadians(InAngle));
	return InRotator + FRotator(InQuat);
}

FRotator OmniMath::RotateAxisX(const FRotator& InRotator, const double InAngle)
{
	//언리얼은 왼손 좌표계+ Z-up. z축만 반대 방향.  
	return RotateAxis(InRotator, InAngle, FVector::ForwardVector * -1);
}

FRotator OmniMath::RotateAxisY(const FRotator& InRotator, const double InAngle)
{
	return RotateAxis(InRotator, InAngle, FVector::RightVector * -1);
}

FRotator OmniMath::RotateAxisZ(const FRotator& InRotator, const double InAngle)
{
	return RotateAxis(InRotator, InAngle, FVector::UpVector);
}

double OmniMath::GetBoxWidth(const FBox& InBox)
{
	return InBox.Max.Y - InBox.Min.Y;
}

double OmniMath::RoundHalfToEvenByUnitValue(const double InDouble, const double InUnitValue /*= 1.0*/)
{
	return FMath::RoundHalfToEven(InDouble / InUnitValue) * InUnitValue;
};

FVector OmniMath::RoundHalfToEvenVector(const FVector& InVector, const double InUnitValue /*= 1.0*/)
{
	return FVector(RoundHalfToEvenByUnitValue(InVector.X, InUnitValue)
	             , RoundHalfToEvenByUnitValue(InVector.Y, InUnitValue)
	             , RoundHalfToEvenByUnitValue(InVector.Z, InUnitValue));
};
