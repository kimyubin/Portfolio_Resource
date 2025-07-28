// Fill out your copyright notice in the Description page of Project Settings.
#include "EnochGameInstance.h"

#include "Enoch.h"
#include "Common/SkillData.h"

UEnochGameInstance::UEnochGameInstance()
{
	IsInitDataComplete = false;
}
void UEnochGameInstance::Init()
{
	Super::Init();

}

void UEnochGameInstance::Shutdown()
{
	UninitData();
	
	Super::Shutdown();
}

bool UEnochGameInstance::InitData()
{
	if(IsInitDataComplete)
		return true;
	// 에디터, 릴리즈 구분해야함
	FString cwd = FString(WCHAR_TO_TCHAR(GetEnochDataPath().c_str()));
	
	UE_LOG(Enoch, Log, TEXT("Start Init Data - Working Dir : %s"), *cwd);
	
	// Skill, Alliance, FreeLancer 순으로 초기화 필요
	if (!SkillData::InitSkillTemplate(*cwd))
	{
		UE_LOG(Enoch, Log, TEXT("Skill Template Data Load Fail"));
		return false;
	}
	if (!AllianceTemplate::InitAllianceTemplate(*cwd))
	{
		UE_LOG(Enoch, Log, TEXT("Alliance Template Data Load Fail"));
		return false;
	}
	//Todo: mac os 컴파일 커밋에서 문제가 된부분
	//if (!FreeLancerTemplate::InitFreeLancerTemplate(TCHAR_TO_WCHAR(&cwd)) )
	if (!FreeLancerTemplate::InitFreeLancerTemplate(*cwd) )
	{
		UE_LOG(Enoch, Log, TEXT("FreeLancer Template Data Load Fail"));
		return false;
	}	
	
	commander = NewObject<UCommanderWrapper>();
	commander->InitCommander();
	bSoloMode = true;
	SelectedFreeLancerInfo = FSelectedFreelancer();
	commander->ReqUserInfo();

	UE_LOG(Enoch, Log, TEXT("End Init Data"));
	IsInitDataComplete = true;
	return true;
}

void UEnochGameInstance::UninitData()
{
	UE_LOG(Enoch, Log, TEXT("Start Uninit Data"));
}
void UEnochGameInstance::SelectedFreelancerUpdate(FSelectedFreelancer val)
{	
	SelectedFreeLancerInfo = val;
}
void UEnochGameInstance::DeselectedFLUpdate()
{
	//내용물 비움
	SelectedFreeLancerInfo = FSelectedFreelancer();
}

void UEnochGameInstance::LiftFreeLancerInField(const FVector2D &CellLocation, AEnochFreeLancer* InFL)
{
	FSelectedFreelancer Select = FSelectedFreelancer(CellLocation,
	                                                 commander->GetFieldFreeLancerList(CellLocation).FLTemplateID,
	                                                 InFL);
	SelectedFreelancerUpdate(Select);
}
