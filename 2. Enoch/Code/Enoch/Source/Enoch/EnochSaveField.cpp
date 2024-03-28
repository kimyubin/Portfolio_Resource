// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochSaveField.h"

#include "EnochGameInstance.h"
#include "Kismet/GameplayStatics.h"

UEnochSaveField::UEnochSaveField()
{
	HeightSize = 10;
	WidthSize = 10;
	
}
void UEnochSaveField::VectorToTArray(const vector<vector<FLSaveData>>& VectorHalf)
{
	FLTemplateIDArray.Empty();
	FLLvArray.Empty();
	for (int i = 0; i < VectorHalf.size(); ++i)
	{
		for (int j = 0; j < VectorHalf[i].size(); ++j)
		{
			FLTemplateIDArray.Push(VectorHalf[i][j].FLTemplateID);
			FLLvArray.Push(VectorHalf[i][j].FLLevel);	
		}
	}	
}
vector<vector<FLSaveData>> UEnochSaveField::TarrayToVector()
{
	vector<vector<FLSaveData>> result;
	int idx = 0;
	for (int i = 0; i < HeightSize/2; ++i)
	{
		result.push_back(vector<FLSaveData>());
		for (int j = 0; j < WidthSize; ++j)
		{
			result.back().emplace_back(FLTemplateIDArray[idx],FLLvArray[idx]);
			++idx;
		}
	}
	return result;
}

void UEnochSaveField::SetSaveData(int SaveSlotNum, UEnochGameInstance* EGameInstance)
{	
	//아군 필드 half리스트에 저장
	EGameInstance->commander->FriendToHalf();
	HeightSize = EnochFieldData::GetHeight();
	WidthSize = EnochFieldData::GetWidth();
	VectorToTArray(EGameInstance->commander->GetHalfField());
	
	FString SaveName = FString(("Field_" + to_string(SaveSlotNum)).c_str());
	UGameplayStatics::SaveGameToSlot(this,SaveName,0);
}
void UEnochSaveField::GetLoadDataToEnemy( UEnochGameInstance* EGameInstance)
{	
	//halfList에 로드
	EGameInstance->commander->SetHalfField(TarrayToVector());
	//적군 필드에 붙이기
	EGameInstance->commander->HalfToEnemy();
	//필드 리셋
	EGameInstance->MyEnochField->GenerateField();
}
void UEnochSaveField::GetLoadDataToFriend(UEnochGameInstance* EGameInstance)
{
	//halfList에 로드
	EGameInstance->commander->SetHalfField(TarrayToVector());
	//아군 필드에 붙이기
	EGameInstance->commander->HalfToFriend();
	//필드 리셋
	EGameInstance->MyEnochField->GenerateField();
}
