// // Fill out your copyright notice in the Description page of Project Settings.
//
// #include "OmnibusUtilities.h"
//
// #include <chrono>
// #include <iomanip>
// #include <sstream>
// #include <string>
// #include <bitset>
//
// #include "OmnibusTypes.h"
// #include "Omnibus.h"
// #include "AssetRegistry/AssetRegistryModule.h"
// #include "Framework/Notifications/NotificationManager.h"
// #include "Kismet/KismetSystemLibrary.h"
// #include "Widgets/Notifications/SNotificationList.h"
//
// #if WITH_EDITOR
// #include "Components/TextRenderComponent.h"
// #include "Engine/TextRenderActor.h"
// #endif // WITH_EDITOR
//
// uint64 FUtlTime::LevelStartTime_Sec = 0;
//
// void UtlMsg::Dialog(const FString& InMsg)
// {
// 	FMessageDialog::Debugf(FText::FromString(InMsg));
// }
//
// void UtlMsg::ToastMsg(const FString& InMsg)
// {
// 	const FText MsgTxt = FText::FromString(InMsg);
// 	const FNotificationInfo ErrorNotification(MsgTxt);
// 	FSlateNotificationManager::Get().AddNotification(ErrorNotification);
// }
//
// ATextRenderActor* SimpleTextRender(const FVector& InLocation, const float InLifeSpan, const FString& InText, const float InWorldSize, const double InZOffset/* = 10*/, const FColor& InRenderColor/* = FColor::Red*/)
// {
// #if WITH_EDITOR
// 	UWorld* World;
//
// 	if (GIsEditor)
// 	{
// 		World = GWorld;
// 	}
// 	else
// 	{
// 		World = GEngine ? GEngine->GetWorldContexts()[0].World() : nullptr;
// 	}
// 	UT_IF(World == nullptr)
// 		return nullptr;
//
// 	ATextRenderActor* TextRender = World->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), InLocation + FVector(0, 0, InZOffset), FRotator(90.0, 0.0, 0.0));
// 	TextRender->SetLifeSpan(InLifeSpan);
// 	TextRender->GetTextRender()->SetText(FText::FromString(InText));
// 	TextRender->GetTextRender()->SetWorldSize(InWorldSize);
// 	TextRender->GetTextRender()->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
// 	TextRender->GetTextRender()->SetVerticalAlignment(EVerticalTextAligment::EVRTA_TextCenter);
// 	TextRender->GetTextRender()->SetTextRenderColor(InRenderColor);
//
// 	return TextRender;
// #else
// 	return nullptr;
// #endif // WITH_EDITOR
// }
//
//
// FName UtlStr::ConcatStrInt(const FString& InNameString, const int32 InIdx)
// {
// 	std::ostringstream oss;
// 	oss << std::setw(2) << std::setfill('0') << InIdx;
// 	const FString TwoDigitIdxStr = oss.str().c_str();
//
// 	return FName(InNameString + "_" + TwoDigitIdxStr);
// }
//
// FString UtlStr::EnumToString(const ECityBlockType InBlockType)
// {
// 	switch (InBlockType)
// 	{
// 		case ECityBlockType::None:
// 			return "None";
// 		case ECityBlockType::Redevelopment:
// 			return "Redevelopment";
// 		case ECityBlockType::Residence:
// 			return "Residence";
// 		case ECityBlockType::Commerce:
// 			return "Commerce";
// 		case ECityBlockType::Industry:
// 			return "Industry";
// 		case ECityBlockType::Tour:
// 			return "Tour";
// 		case ECityBlockType::GreenSpace:
// 			return "GreenSpace";
// 		case ECityBlockType::Size:
// 			return "Size";
// 	}
// 	return "";
// }
//
// uint32 UtlMath::CircularNum(const uint32 InMax, const int64 InNum)
// {
// 	// InMax를 범위에 포함하기 위해 1 더함.
// 	const int64 MaxOutLine = static_cast<int64>(InMax) + 1;
// 	if (InNum < 0)
// 		return ((InNum % MaxOutLine) + MaxOutLine) % MaxOutLine;
// 	else if (InNum <= InMax)
// 		return InNum;
//
// 	return InNum % MaxOutLine;
// }
//
// double UtlMath::CircularNumF(const double InMax, const double InNum)
// {
// 	// 1더하지 않아도, InMax는 경계에 포함
// 	const double MaxOutLine = InMax;
// 	if (InNum < 0)
// 		return fmod(fmod(InNum, MaxOutLine) + MaxOutLine, MaxOutLine);
// 	else if (InNum <= InMax)
// 		return InNum;
// 	
// 	return fmod(InNum, MaxOutLine);
// }
//
// FTransform UtlMath::GetTransformAddOffset(const FVector& InPos, const FVector& InDirection, const double InOffset)
// {
// 	const FVector ResVector = InPos + InDirection * InOffset;
// 	return FTransform(InDirection.Rotation(), ResVector);
// }
//
// void FUtlTime::SetLevelStartTime_Sec()
// {
// 	LevelStartTime_Sec = GetNowTime_Sec();
// }
//
// uint64 FUtlTime::GetLevelStartTime_Sec()
// {
// 	if (LevelStartTime_Sec == 0)
// 		SetLevelStartTime_Sec();
//
// 	return LevelStartTime_Sec;
// }
//
// uint64 FUtlTime::GetNowTime_Sec()
// {
// 	const FDateTime NowTime = FDateTime::Now();
// 	return static_cast<uint64>(NowTime.GetSecond() + NowTime.GetMinute() * 60 + NowTime.GetHour() * 3600);
// }
//
// int64 FUtlTime::GetNowMicroSec()
// {
// 	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
// }
//
// void FUtlTime::SimpleTimer(const AActor* InWorldActor, const float InRate, TFunction<void()>&& Callback)
// {
// 	FTimerHandle TempTimer;
//
// 	InWorldActor->GetWorldTimerManager().SetTimer(TempTimer, MoveTemp(Callback), InRate, false);
// }
//
// bool FUtlStatics::GetOverlapComps(UPrimitiveComponent* InTestComp, UClass* InComponentClassFilter, TArray<UPrimitiveComponent*>& OutOverlapComps)
// {
// 	const EObjectTypeQuery CollObjectType = UEngineTypes::ConvertToObjectType(InTestComp->GetCollisionObjectType());
//
// 	return UKismetSystemLibrary::ComponentOverlapComponents(InTestComp
// 	                                                      , InTestComp->GetComponentTransform()
// 	                                                      , {CollObjectType}
// 	                                                      , InComponentClassFilter
// 	                                                      , TArray<AActor*>()
// 	                                                      , OutOverlapComps);
// }
//
// bool FUtlStatics::GetOverlapActors(UPrimitiveComponent* InTestComp, UClass* InActorClassFilter, TArray<AActor*>& OutOverlapActors)
// {
// 	EObjectTypeQuery CollObjectTypes = UEngineTypes::ConvertToObjectType(InTestComp->GetCollisionObjectType());
//
// 	return UKismetSystemLibrary::ComponentOverlapActors(InTestComp
// 	                                                  , InTestComp->GetComponentTransform()
// 	                                                  , {CollObjectTypes}
// 	                                                  , InActorClassFilter
// 	                                                  , TArray<AActor*>()
// 	                                                  , OutOverlapActors);
// }
//
// FAssetData FUtlStatics::GetAssetByObjectPath(const FSoftObjectPath& ObjectPath, bool bIncludeOnlyOnDiskAssets, bool bSkipARFilteredAssets)
// {
// 	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
//
// 	return AssetRegistryModule.Get().GetAssetByObjectPath(ObjectPath, bIncludeOnlyOnDiskAssets, bSkipARFilteredAssets);
// }
//
// bool FUtlStatics::IsAssetExists(const FSoftObjectPath& ObjectPath)
// {
// 	return GetAssetByObjectPath(ObjectPath).IsValid();
// }
//
// bool FUtlStatics::CreateUniqueAssetName(const FString& ObjectPath, FString& OutAssetPathName)
// {
// 	// 이미 해당 경로에 애셋이 있는지 확인.
// 	bool bAssetExists  = true;
// 	FString NewName    = ObjectPath;
//
// 	constexpr int Loop = 1000;
// 	for (int count = 0; count < Loop; ++count)
// 	{
// 		bAssetExists = IsAssetExists(NewName);
// 		if (bAssetExists == false)
// 			break;
//
// 		NewName = ObjectPath + "_" + FString::FromInt(count);
// 	}
//
// 	// 중복된 이름. 이미 에셋 이름이 존재합니다.
// 	if (bAssetExists)
// 	{
// 		UtlMsg::ToastMsg("Duplicate name. The asset name already exists.");
// 		return false;
// 	}
//
// 	OutAssetPathName = NewName;
// 	return true;
// }
//
// uint64 UtlBit::ExtractFillBit(const uint64 InBitVal, const uint64 InStartBitOffset, const uint64 InBitCount)
// {
// 	if (InStartBitOffset > 63 || InBitCount > 64 || InStartBitOffset + InBitCount > 64)
// 	{
// 		UT_LOG_STR("Bit Range Over")
// 		return 0;
// 	}
// 	
// 	//하위 비트를 BitCount만큼 추출
// 	int64 res = InBitVal & ((static_cast<uint64>(1) << InBitCount) - 1);
// 	//추출한 비트를 시작 지점까지 이동
// 	res = res << (63 - (InStartBitOffset + InBitCount - 1));
//
// 	return res;
// }
//
// uint64 UtlBit::GetSubBit(const uint64 InBitValue, const uint64 InStartBitOffset, const uint64 InBitCount)
// {
// 	if (InStartBitOffset > 63 || InBitCount > 64 || (InStartBitOffset + InBitCount) > 64)
// 	{
// 		UT_LOG_STR("Bit Range Over")
// 		return 0;
// 	}
//
// 	//InStartBitOffset번째 비트부터 InBitCount만큼 마스킹
// 	int64 res = InBitValue >> (63 - (InStartBitOffset + InBitCount - 1));
// 	res &= (static_cast<uint64>(1) << InBitCount) - 1;
//
// 	return res;
// }
//
// int64 UtlBit::GetSignedBitRange(const uint64 InBitValue, const uint64 InStartBit, const uint64 InBitCount)
// {
// 	if (InStartBit > 63 || InBitCount > 64 || InStartBit + InBitCount > 64)
// 	{
// 		UT_LOG_STR("Bit Range Over")
// 		return 0;
// 	}
//
// 	//InStartBit번째 비트부터 InBitCount만큼 마스킹
// 	int64 res = InBitValue >> (63 - (InStartBit + InBitCount - 1));
// 	res &= (static_cast<uint64>(1) << InBitCount) - 1;
//
// 	// 음수일 경우, 상위 비트 1로 채움
// 	if ((res >> (InBitCount - 1)) == 1)
// 	{
// 		const uint64 All1 = ~static_cast<uint64>(0) << InBitCount;
// 		res = res | All1;
// 	}
//
// 	return res;
// }
//
// FVector2D UtlMath::MakeFVector2D(const FVector& InVector)
// {
// 	return FVector2D(InVector.X, InVector.Y);
// }
//
// FVector UtlMath::MakeFVector(const FVector2D& InVector, const double InZ/*= 0.0*/)
// {
// 	return FVector(InVector.X, InVector.Y, InZ);
// }
//
// FVector2D UtlMath::ClampVector2D(const FVector2D& InVector2D, const FVector2D& A, const FVector2D& B)
// {
// 	const FVector2D InMin = FVector2D(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y));
// 	const FVector2D InMax = FVector2D(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y));
//
// 	return FVector2D(FMath::Clamp(InVector2D.X, InMin.X, InMax.X)
// 	               , FMath::Clamp(InVector2D.Y, InMin.Y, InMax.Y));
// }
//
// FVector UtlMath::ClampVector(const FVector& InVector, const FVector& A, const FVector& B)
// {
// 	const FVector InMin = FVector(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), InVector.Z);
// 	const FVector InMax = FVector(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), InVector.Z);
//
// 	return FVector(FMath::Clamp(InVector.X, InMin.X, InMax.X)
// 	             , FMath::Clamp(InVector.Y, InMin.Y, InMax.Y)
// 	             , InVector.Z);
// }
//
// FRotator UtlMath::RotateAxis(const FRotator& InRotator, const double InAngle, const FVector& InAxis)
// {
// 	//입력 축을 기준으로 입력 각도만큼 회전한 쿼터니언을 구함.	
// 	const FQuat InQuat = FQuat(InAxis.GetSafeNormal(), FMath::DegreesToRadians(InAngle));
// 	return InRotator + FRotator(InQuat);
// }
//
// FRotator UtlMath::RotateAxisX(const FRotator& InRotator, const double InAngle)
// {
// 	//언리얼은 왼손 좌표계+ Z-up. z축만 반대 방향.  
// 	return RotateAxis(InRotator, InAngle, FVector::ForwardVector * -1);
// }
//
// FRotator UtlMath::RotateAxisY(const FRotator& InRotator, const double InAngle)
// {
// 	return RotateAxis(InRotator, InAngle, FVector::RightVector * -1);
// }
//
// FRotator UtlMath::RotateAxisZ(const FRotator& InRotator, const double InAngle)
// {
// 	return RotateAxis(InRotator, InAngle, FVector::UpVector);
// }
//
// double UtlMath::GetBoxWidth(const FBox& InBox)
// {
// 	return InBox.Max.Y - InBox.Min.Y;
// }
//
// double UtlMath::RoundHalfToEvenByUnitValue(const double InDouble, const double InUnitValue /*= 1.0*/)
// {
// 	return FMath::RoundHalfToEven(InDouble / InUnitValue) * InUnitValue;
// }
//
// FVector UtlMath::RoundHalfToEvenVector(const FVector& InVector, const double InUnitValue /*= 1.0*/)
// {
// 	return FVector(RoundHalfToEvenByUnitValue(InVector.X, InUnitValue)
// 	             , RoundHalfToEvenByUnitValue(InVector.Y, InUnitValue)
// 	             , RoundHalfToEvenByUnitValue(InVector.Z, InUnitValue));
// }
//
// FString UtlMath::ToHex(const uint64 InInt)
// {
// 	return FString::Printf(TEXT("%16llx"), InInt);
// }
//
// FString UtlMath::ToHex(const uint32 InInt)
// {
// 	return FString::Printf(TEXT("%8lx"), InInt);
// }
//
// FString UtlMath::ToBin(const uint64 InInt)
// {
// 	return std::bitset<64>(InInt).to_string().c_str();
// }
//
// FString UtlMath::ToBin(const uint32 InInt)
// {
// 	return std::bitset<32>(InInt).to_string().c_str();
// }
