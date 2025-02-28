// Fill out your copyright notice in the Description page of Project Settings.

#include "UTLStatics.h"

#include "Utilibrary.h"
#include "UtlLog.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Notifications/SNotificationList.h"

#include <bitset>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

#if WITH_EDITOR
#include "Components/TextRenderComponent.h"
#include "Engine/TextRenderActor.h"
#endif // WITH_EDITOR


uint64 FUtlTime::LevelStartTime_Sec = 0;

void UtlMsg::Dialog(const FString& InMsg)
{
	FMessageDialog::Debugf(FText::FromString(InMsg));
}

void UtlMsg::ToastMsg(const FString& InMsg)
{
	const FText MsgTxt = FText::FromString(InMsg);
	const FNotificationInfo ErrorNotification(MsgTxt);
	FSlateNotificationManager::Get().AddNotification(ErrorNotification);
}

ATextRenderActor* FUtlDebug::SimpleTextRender(const FVector& InLocation, const float InLifeSpan, const FString& InText, const float InWorldSize, const double InZOffset/* = 10*/, const FColor& InRenderColor/* = FColor::Red*/)
{
#if WITH_EDITOR
	UWorld* World;

	if (GIsEditor)
	{
		World = GWorld;
	}
	else
	{
		World = GEngine ? GEngine->GetWorldContexts()[0].World() : nullptr;
	}
	UT_IF(World == nullptr)
		return nullptr;

	ATextRenderActor* TextRender = World->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), InLocation + FVector(0, 0, InZOffset), FRotator(90.0, 0.0, 0.0));
	TextRender->SetLifeSpan(InLifeSpan);
	TextRender->GetTextRender()->SetText(FText::FromString(InText));
	TextRender->GetTextRender()->SetWorldSize(InWorldSize);
	TextRender->GetTextRender()->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	TextRender->GetTextRender()->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
	TextRender->GetTextRender()->SetTextRenderColor(InRenderColor);

	return TextRender;
#else
	return nullptr;
#endif // WITH_EDITOR
}

void FUtlDebug::SimpleSphere(const FVector& InLocation
                           , const float InRadius
                           , const float InLifeTime
                           , const bool bPersistentLines
                           , const FColor& InRenderColor
                           , const int32 InSegments
                           , const uint8 InDepthPriority
                           , const float InThickness)
{
#if WITH_EDITOR
	UWorld* World;

	if (GIsEditor)
	{
		World = GWorld;
	}
	else
	{
		World = GEngine ? GEngine->GetWorldContexts()[0].World() : nullptr;
	}
	UT_IF(World == nullptr)
		return;

	DrawDebugSphere(World, InLocation, InRadius, InSegments, InRenderColor, bPersistentLines, InLifeTime, InDepthPriority, InThickness);

#else

#endif // WITH_EDITOR
}


FName UtlStr::ConcatStrInt(const FString& InNameString, const int32 InIdx)
{
	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << InIdx;
	const FString TwoDigitIdxStr = oss.str().c_str();

	return FName(InNameString + "_" + TwoDigitIdxStr);
}

uint32 UtlMath::CircularNum(const uint32 InMax, const int64 InNum)
{
	// InMax를 범위에 포함하기 위해 1 더함.
	const int64 MaxOutLine = static_cast<int64>(InMax) + 1;
	if (InNum < 0)
		return ((InNum % MaxOutLine) + MaxOutLine) % MaxOutLine;
	else if (InNum <= InMax)
		return InNum;

	return InNum % MaxOutLine;
}

double UtlMath::CircularNumF(const double InMax, const double InNum)
{
	// 1더하지 않아도, InMax는 경계에 포함
	const double MaxOutLine = InMax;
	if (InNum < 0)
		return fmod(fmod(InNum, MaxOutLine) + MaxOutLine, MaxOutLine);
	else if (InNum <= InMax)
		return InNum;
	
	return fmod(InNum, MaxOutLine);
}

FTransform UtlMath::GetTransformAddOffset(const FVector& InPos, const FVector& InDirection, const double InOffset)
{
	const FVector ResVector = InPos + InDirection * InOffset;
	return FTransform(InDirection.Rotation(), ResVector);
}

void FUtlTime::SetLevelStartTime_Sec()
{
	LevelStartTime_Sec = GetNowTime_Sec();
}

uint64 FUtlTime::GetLevelStartTime_Sec()
{
	if (LevelStartTime_Sec == 0)
		SetLevelStartTime_Sec();

	return LevelStartTime_Sec;
}

uint64 FUtlTime::GetNowTime_Sec()
{
	const FDateTime NowTime = FDateTime::Now();
	return static_cast<uint64>(NowTime.GetSecond() + NowTime.GetMinute() * 60 + NowTime.GetHour() * 3600);
}

int64 FUtlTime::GetNowMilliSec()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64 FUtlTime::GetNowMicroSec()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void FUtlTime::SimpleTimer(const AActor* InWorldActor, const float InRate, TFunction<void()>&& Callback)
{
	FTimerHandle TempTimer;

	InWorldActor->GetWorldTimerManager().SetTimer(TempTimer, MoveTemp(Callback), InRate, false);
}

bool FUtlStatics::GetOverlapComps(UPrimitiveComponent* InTestComp, UClass* InComponentClassFilter, TArray<UPrimitiveComponent*>& OutOverlapComps)
{
	const EObjectTypeQuery CollObjectType = UEngineTypes::ConvertToObjectType(InTestComp->GetCollisionObjectType());

	return UKismetSystemLibrary::ComponentOverlapComponents(InTestComp
	                                                      , InTestComp->GetComponentTransform()
	                                                      , {CollObjectType}
	                                                      , InComponentClassFilter
	                                                      , TArray<AActor*>()
	                                                      , OutOverlapComps);
}

bool FUtlStatics::GetOverlapActors(UPrimitiveComponent* InTestComp, UClass* InActorClassFilter, TArray<AActor*>& OutOverlapActors)
{
	EObjectTypeQuery CollObjectTypes = UEngineTypes::ConvertToObjectType(InTestComp->GetCollisionObjectType());

	return UKismetSystemLibrary::ComponentOverlapActors(InTestComp
	                                                  , InTestComp->GetComponentTransform()
	                                                  , {CollObjectTypes}
	                                                  , InActorClassFilter
	                                                  , TArray<AActor*>()
	                                                  , OutOverlapActors);
}

FAssetData FUtlStatics::GetAssetByObjectPath(const FSoftObjectPath& ObjectPath, bool bIncludeOnlyOnDiskAssets, bool bSkipARFilteredAssets)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	return AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath, bIncludeOnlyOnDiskAssets, bSkipARFilteredAssets);
}

bool FUtlStatics::IsAssetExists(const FSoftObjectPath& ObjectPath)
{
	return GetAssetByObjectPath(ObjectPath).IsValid();
}

bool FUtlStatics::CreateUniqueAssetName(const FString& ObjectPath, FString& OutAssetPathName)
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
		UtlMsg::ToastMsg("Duplicate name. The asset name already exists.");
		return false;
	}

	OutAssetPathName = NewName;
	return true;
}

uint64 UtlBit::ExtractFillBit(const uint64 InBitVal, const uint64 InStartBitOffset, const uint64 InBitCount)
{
	if (InStartBitOffset > 63 || InBitCount > 64 || InStartBitOffset + InBitCount > 64)
	{
		UT_LOG("Bit Range Over")
		return 0;
	}
	
	//하위 비트를 BitCount만큼 추출
	int64 res = InBitVal & ((static_cast<uint64>(1) << InBitCount) - 1);
	//추출한 비트를 시작 지점까지 이동
	res = res << (63 - (InStartBitOffset + InBitCount - 1));

	return res;
}

uint64 UtlBit::GetSubBit(const uint64 InBitValue, const uint64 InStartBitOffset, const uint64 InBitCount)
{
	if (InStartBitOffset > 63 || InBitCount > 64 || (InStartBitOffset + InBitCount) > 64)
	{
		UT_LOG("Bit Range Over")
		return 0;
	}

	//InStartBitOffset번째 비트부터 InBitCount만큼 마스킹
	int64 res = InBitValue >> (63 - (InStartBitOffset + InBitCount - 1));
	res &= (static_cast<uint64>(1) << InBitCount) - 1;

	return res;
}

int64 UtlBit::GetSignedBitRange(const uint64 InBitValue, const uint64 InStartBit, const uint64 InBitCount)
{
	if (InStartBit > 63 || InBitCount > 64 || InStartBit + InBitCount > 64)
	{
		UT_LOG("Bit Range Over")
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

FVector2D UtlMath::MakeFVector2D(const FVector& InVector)
{
	return FVector2D(InVector.X, InVector.Y);
}

FVector UtlMath::MakeFVector(const FVector2D& InVector, const double InZ/*= 0.0*/)
{
	return FVector(InVector.X, InVector.Y, InZ);
}

FVector2D UtlMath::ClampVector2D(const FVector2D& InVector2D, const FVector2D& A, const FVector2D& B)
{
	const FVector2D InMin = FVector2D(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y));
	const FVector2D InMax = FVector2D(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y));

	return FVector2D(FMath::Clamp(InVector2D.X, InMin.X, InMax.X)
	               , FMath::Clamp(InVector2D.Y, InMin.Y, InMax.Y));
}

FVector UtlMath::ClampVector(const FVector& InVector, const FVector& A, const FVector& B)
{
	const FVector InMin = FVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), InVector.Z);
	const FVector InMax = FVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), InVector.Z);

	return FVector(FMath::Clamp(InVector.X, InMin.X, InMax.X)
	             , FMath::Clamp(InVector.Y, InMin.Y, InMax.Y)
	             , InVector.Z);
}

FRotator UtlMath::YawRotator(const FRotator& InRotator)
{
	return FRotator(0.0, InRotator.Yaw, 0.0);
}

FRotator UtlMath::RotateAxis(const FRotator& InRotator, const double InAngle, const FVector& InAxis)
{
	const FQuat DeltaQuat     = FQuat(InAxis, FMath::DegreesToRadians(InAngle));
	const FQuat CurRelRotQuat = InRotator.Quaternion();

	//입력 축을 기준으로 입력 각도만큼 회전한 쿼터니언
	const FQuat NewRelRotQuat = CurRelRotQuat * DeltaQuat;

	return NewRelRotQuat.Rotator();
}

FRotator UtlMath::RotateAxisX(const FRotator& InRotator, const double InAngle)
{
	//언리얼은 왼손 좌표계+ Z-up. z축만 반대 방향.  
	return RotateAxis(InRotator, InAngle, FVector::ForwardVector * -1);
}

FRotator UtlMath::RotateAxisY(const FRotator& InRotator, const double InAngle)
{
	return RotateAxis(InRotator, InAngle, FVector::RightVector * -1);
}

FRotator UtlMath::RotateAxisZ(const FRotator& InRotator, const double InAngle)
{
	return RotateAxis(InRotator, InAngle, FVector::UpVector);
}

double UtlMath::GetBoxWidth(const FBox& InBox)
{
	return InBox.Max.Y - InBox.Min.Y;
}

double UtlMath::RoundHalfToEvenByUnitValue(const double InDouble, const double InUnitValue /*= 1.0*/)
{
	return FMath::RoundHalfToEven(InDouble / InUnitValue) * InUnitValue;
}

FVector UtlMath::RoundHalfToEvenVector(const FVector& InVector, const double InUnitValue /*= 1.0*/)
{
	return FVector(RoundHalfToEvenByUnitValue(InVector.X, InUnitValue)
	             , RoundHalfToEvenByUnitValue(InVector.Y, InUnitValue)
	             , RoundHalfToEvenByUnitValue(InVector.Z, InUnitValue));
}

FVector UtlMath::RoundVector(const FVector& InVector, const double InPrecision)
{
	return FVector(FMath::RoundToDouble(InVector.X / InPrecision) * InPrecision
	             , FMath::RoundToDouble(InVector.Y / InPrecision) * InPrecision
	             , FMath::RoundToDouble(InVector.Z / InPrecision) * InPrecision);
}

FVector UtlMath::FloorVector(const FVector& InVector)
{
	return FVector(FMath::FloorToDouble(InVector.X)
	             , FMath::FloorToDouble(InVector.Y)
	             , FMath::FloorToDouble(InVector.Z));
}

FIntVector UtlMath::TruncToIntVector(const FVector& InVec, const double InPrecision)
{
	return FIntVector(FMath::TruncToInt(InVec.X / InPrecision) * InPrecision
	                , FMath::TruncToInt(InVec.Y / InPrecision) * InPrecision
	                , FMath::TruncToInt(InVec.Z / InPrecision) * InPrecision);
}

FIntVector UtlMath::FloorToIntVector(const FVector& InVec, const double InPrecision)
{
	return FIntVector(FMath::FloorToInt(InVec.X / InPrecision) * InPrecision
	                , FMath::FloorToInt(InVec.Y / InPrecision) * InPrecision
	                , FMath::FloorToInt(InVec.Z / InPrecision) * InPrecision);
}

FIntVector UtlMath::RoundToIntVector(const FVector& InVec, const double InPrecision)
{
	return FIntVector(FMath::RoundToInt(InVec.X / InPrecision) * InPrecision
	                , FMath::RoundToInt(InVec.Y / InPrecision) * InPrecision
	                , FMath::RoundToInt(InVec.Z / InPrecision) * InPrecision);
}

FIntVector UtlMath::CeilToIntVector(const FVector& InVec, const double InPrecision)
{
	return FIntVector(FMath::CeilToInt(InVec.X / InPrecision) * InPrecision
	                , FMath::CeilToInt(InVec.Y / InPrecision) * InPrecision
	                , FMath::CeilToInt(InVec.Z / InPrecision) * InPrecision);
}

FIntVector UtlMath::TruncToIntVector(const FVector& InVec)
{
	return FIntVector(FMath::TruncToInt(InVec.X)
	                , FMath::TruncToInt(InVec.Y)
	                , FMath::TruncToInt(InVec.Z));
}

FIntVector UtlMath::FloorToIntVector(const FVector& InVec)
{
	return FIntVector(FMath::FloorToInt(InVec.X)
	                , FMath::FloorToInt(InVec.Y)
	                , FMath::FloorToInt(InVec.Z));
}

FIntVector UtlMath::RoundToIntVector(const FVector& InVec)
{
	return FIntVector(FMath::RoundToInt(InVec.X)
	                , FMath::RoundToInt(InVec.Y)
	                , FMath::RoundToInt(InVec.Z));
}

FIntVector UtlMath::CeilToIntVector(const FVector& InVec)
{
	return FIntVector(FMath::CeilToInt(InVec.X)
	                , FMath::CeilToInt(InVec.Y)
	                , FMath::CeilToInt(InVec.Z));
}

FString UtlMath::ToHex(const uint64 InInt)
{
	return FString::Printf(TEXT("%16llx"), InInt);
}

FString UtlMath::ToHex(const uint32 InInt)
{
	return FString::Printf(TEXT("%8lx"), InInt);
}

FString UtlMath::ToBin(const uint64 InInt)
{
	return std::bitset<64>(InInt).to_string().c_str();
}

FString UtlMath::ToBin(const uint32 InInt)
{
	return std::bitset<32>(InInt).to_string().c_str();
}
