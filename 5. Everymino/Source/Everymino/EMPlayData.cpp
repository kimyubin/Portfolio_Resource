// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "EMPlayData.h"

#include <random>

#include "BrickTemplate.h"
#include "EMController.h"
#include "EMSaveKeySettings.h"
#include "Kismet/GameplayStatics.h"

AEMPlayData::AEMPlayData()
{
	PrimaryActorTick.bCanEverTick = false;

	CenterWidth = 12;
	ShaftWidth = 8;
	ShaftHeight = 12;
	GameBoardWidth = ShaftHeight*2+CenterWidth;
	VanishZone = 10;
	//스킨 목록 기본틀 생성.
	for(int i = 0; i < EnumToInt(UnitSkin::Size); ++i)
	{
		FUnitSkinData temp = FUnitSkinData(static_cast<UnitSkin>(i));
		UnitSkinMeshes.Push(temp);
	}	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Skin(TEXT("/Game/Cube_01.Cube_01"));
	UnitSkinMeshes[EnumToInt(UnitSkin::Default)].StaticMesh = Skin.Object;

	//기준 시간 초기화
	DelayedAutoShift = 0.1f;
	AutoRepeatRate = 0.05f;
	SoftDropFactor = 0.1f;
	AutoFallingSpeed = 0.8f;
	LockdownDelay = 0.f;
	LockdownCount = 0.f;
	AerialLockdownCount = 0.f;
	
}

void AEMPlayData::BeginPlay()
{
	Super::BeginPlay();
	
	MyBricksSkinData = BrickTemplate::GetBrickShapes();
	CustomGameBoard = GetNewGameBoard(CenterWidth, ShaftWidth, ShaftHeight);
}

vector<vector<UnitSkin>> AEMPlayData::GetBrickShape(BrickType BrickT, BrickDirection Dir)
{
	return MyBricksSkinData[EnumToInt(BrickT)][EnumToInt(Dir)];
}
vector<vector<UnitSkin>> AEMPlayData::GetCurrentGameBoard()
{
	return CustomGameBoard;
}
vector<vector<UnitSkin>> AEMPlayData::GetNewGameBoard(uint16 InCenterWidth, uint16 InShaftWidth, uint16 InShaftHeight)
{
	CenterWidth = InCenterWidth;
	ShaftWidth = InShaftWidth;
	ShaftHeight = InShaftHeight;
	GameBoardWidth = ShaftHeight*2+CenterWidth;
	
	uint64 hash_key = CenterWidth;
	hash_key <<= 16;
	hash_key += ShaftWidth;
	hash_key <<= 16;
	hash_key += ShaftHeight;
	auto it = CustomGameBoardMap.find(hash_key);
	if(it==CustomGameBoardMap.end())
	{
		CustomGameBoard = GenerateGameBoard(CenterWidth, ShaftWidth, ShaftHeight);
		CustomGameBoardMap.emplace(hash_key, CustomGameBoard);		
	}
	else
		CustomGameBoard = it->second;

	return CustomGameBoard;
}
vector<vector<UnitSkin>> AEMPlayData::GenerateGameBoard(uint16 InCenterWidth, uint16 InShaftWidth, uint16 InShaftHeight)
{
	//상하 좌우 추가 여백. 브릭이 생성되는 공간
	int TotalWidth = GameBoardWidth;
	TotalWidth += VanishZone*2;

	//샤프트 기준값 및 센터 기준값.
	const int FrontShaftPoint = (TotalWidth - InShaftWidth) / 2 - 1;
	const int BackShaftPoint = (TotalWidth + InShaftWidth) / 2;
	FrontCenterPoint = TotalWidth - InCenterWidth - InShaftHeight - 1 - VanishZone;
	BackCenterPoint = TotalWidth - InShaftHeight - VanishZone;

	//전체 초기화
	vector<vector<UnitSkin>> Res(TotalWidth, vector<UnitSkin>(TotalWidth, UnitSkin::InvisibleWall));

	//shaft벽 생성.
	for (int gbY = 0; gbY < Res.size(); ++gbY)
		for (int gbX = 0; gbX < Res[gbY].size(); ++ gbX)
			if (gbX == FrontShaftPoint || gbX == BackShaftPoint
				|| gbY == FrontShaftPoint || gbY == BackShaftPoint)
				Res[gbY][gbX] = UnitSkin::Wall;

	//center벽 생성.
	for (int gbY = FrontCenterPoint; gbY <= BackCenterPoint; ++gbY)
		for (int gbX = FrontCenterPoint; gbX <= BackCenterPoint; ++ gbX)
			if (gbX == FrontCenterPoint || gbX == BackCenterPoint
				|| gbY == FrontCenterPoint || gbY == BackCenterPoint)
				Res[gbY][gbX] = UnitSkin::Wall;

	//shaft내부 청소
	for (int gbY = 0; gbY < Res.size(); ++gbY)
		for (int gbX = FrontShaftPoint + 1; gbX < BackShaftPoint; ++ gbX)
			Res[gbY][gbX] = UnitSkin::Empty;
	for (int gbY = FrontShaftPoint + 1; gbY < BackShaftPoint; ++gbY)
		for (int gbX = 0; gbX < Res.size(); ++ gbX)
			Res[gbY][gbX] = UnitSkin::Empty;

	//center내부 청소
	for (int gbY = FrontCenterPoint + 1; gbY <= BackCenterPoint - 1; ++gbY)
		for (int gbX = FrontCenterPoint + 1; gbX <= BackCenterPoint - 1; ++ gbX)
			Res[gbY][gbX] = UnitSkin::Empty;
	

	return Res;
}
FVector2I AEMPlayData::GetDropStartPoint(EDropStartDirection InStartDirection)
{
	FVector2I Res;
	const int TotalLength = (GameBoardWidth + (VanishZone * 2));
	switch (InStartDirection)
	{
	case EDropStartDirection::North:
		//전체 길이+여백공간의 한가운데. 브릭이 4칸짜리라 2칸 옆으로 밈
		Res.X = TotalLength / 2 - 2;
		//여백에서 브릭이 차지하는 2칸 제외.
		Res.Y = VanishZone - 2;
		break;
	case EDropStartDirection::East:
		Res.X = TotalLength - VanishZone - 1;
		Res.Y = TotalLength / 2 - 2;
		break;
	case EDropStartDirection::South:
		Res.X = TotalLength / 2 - 2;
		Res.Y = TotalLength - VanishZone - 1;
		break;
	case EDropStartDirection::West:
		Res.X = VanishZone - 2;
		Res.Y = TotalLength / 2 - 2;
		break;
	default:
		break;
	}

	
	return Res;	
}

void AEMPlayData::ChangeAllBricksSkin(UnitSkin Skin)
{
	if (Skin == UnitSkin::Size)
		return;
	
	for(int i = 0; i < EnumToInt(BrickType::Size); ++i)
		ChangeBrickSkin(static_cast<BrickType>(i), Skin);	
}
void AEMPlayData::ChangeBrickSkin(BrickType TargetBrickT, UnitSkin Skin)
{
	if (TargetBrickT == BrickType::Size || Skin == UnitSkin::Size)
		return;
	
	for (auto& i : MyBricksSkinData[EnumToInt(TargetBrickT)])
		for (auto& j : i)
			for (auto& k : j)
				if (k != UnitSkin::Empty)
					k = Skin;	
}

void AEMPlayData::ChangeUnitSkin(BrickType TargetBrickT, uint8 TargetUnitNum, UnitSkin Skin)
{
	if (TargetBrickT == BrickType::Size || Skin == UnitSkin::Size || TargetUnitNum >= BrickTemplate::MAX_UNIT_NUM)
		return;
	
	vector<vector<vector<UnitSkin>>>& BrickDirections = MyBricksSkinData[EnumToInt(TargetBrickT)];
	vector<vector<UnitSkin>>* Brick;

	//비교하고 있는 현재 유닛 번호
	int UnitCount = 0;
	
	auto ChangeSkin = [&](int& y, int& x)-> bool
	{		
		if ((*Brick)[y][x] != UnitSkin::Empty)
		{			
			if (UnitCount == TargetUnitNum)
			{
				(*Brick)[y][x] = Skin;
				UnitCount = 0;
				return true;
			}
			UnitCount++;
		}
		return false;
	};

	//상. 좌상단->우하단
	Brick = &BrickDirections[EnumToInt(BrickDirection::UP)];
	for (int y = 0; y < (*Brick).size(); ++y)
	{
		bool flag = false;
		for (int x = 0; x < (*Brick).front().size(); ++x)
		{
			flag = ChangeSkin(y, x);
			if (flag) break;
		}
		if (flag) break;
	}
	//우. 우상단 -> 좌하단
	Brick = &BrickDirections[EnumToInt(BrickDirection::RIGHT)];
	for (int x = ((*Brick).front().size() - 1); x >= 0; --x)
	{
		bool flag = false;
		for (int y = 0; y < (*Brick).size(); ++y)
		{
			flag = ChangeSkin(y, x);
			if (flag) break;
		}
		if (flag) break;
	}
	//하. 우하단 -> 좌상단
	Brick = &BrickDirections[EnumToInt(BrickDirection::DOWN)];
	for (int y = ((*Brick).size() - 1); y >= 0; --y)
	{
		bool flag = false;
		for (int x = ((*Brick).front().size() - 1); x >= 0; --x)
		{
			flag = ChangeSkin(y, x);
			if (flag) break;
		}
		if (flag) break;
	}
	//좌. 좌하단 -> 우상단
	Brick = &BrickDirections[EnumToInt(BrickDirection::LEFT)];
	for (int x = 0; x < (*Brick).front().size(); ++x)
	{
		bool flag = false;
		for (int y = ((*Brick).size() - 1); y >= 0; --y)
		{
			flag = ChangeSkin(y, x);
			if (flag) break;
		}
		if (flag) break;
	}
}

void AEMPlayData::ChangeSaveKeySetting(EBindKey::EControlName TargetBindControl, FKey InputKey)
{
	switch (TargetBindControl)
	{
	case EBindKey::EControlName::MoveUp: EBindKey::MoveUp = InputKey;				break;
	case EBindKey::EControlName::MoveRight: EBindKey::MoveRight = InputKey;			break;
	case EBindKey::EControlName::MoveDown: EBindKey::MoveDown = InputKey;			break;
	case EBindKey::EControlName::MoveLeft: EBindKey::MoveLeft = InputKey;			break;
	case EBindKey::EControlName::Clockwise: EBindKey::Clockwise = InputKey;			break;
	case EBindKey::EControlName::AntiClockwise: EBindKey::AntiClockwise = InputKey;	break;
	case EBindKey::EControlName::SelectNorth: EBindKey::SelectNorth = InputKey;		break;
	case EBindKey::EControlName::SelectEast: EBindKey::SelectEast = InputKey;		break;
	case EBindKey::EControlName::SelectSouth: EBindKey::SelectSouth = InputKey;		break;
	case EBindKey::EControlName::SelectWest: EBindKey::SelectWest = InputKey;		break;
	default: break;
	}
	
	Cast<AEMController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->SetupBoardManagerInput();
	
	//셋팅 세이브
	UEMSaveKeySettings* SaveObj = NewObject<UEMSaveKeySettings>();
	SaveObj->SaveKeySettings();
}

void AEMPlayData::LoadKeySettings()
{
	UEMSaveKeySettings* LoadObj = Cast<UEMSaveKeySettings>(UGameplayStatics::LoadGameFromSlot(FString("KeySettings"), 0));
	if(LoadObj!=nullptr)
		LoadObj->LoadKeySettings();

}


UStaticMesh* AEMPlayData::GetUnitSkinMesh(UnitSkin Skin)
{
	uint8 SkinNum = EnumToInt(Skin);
	if(UnitSkinMeshes.Num()<=SkinNum)
		return nullptr;
	return UnitSkinMeshes[SkinNum].StaticMesh;
}

BrickType AEMPlayData::GetNewBrickType()
{
	//7개 이하면 14개 셔플해서 추가. 셔플백 사이즈 변경 대비해서 함수로 처리
	if(BrickBag.size()<7)
	{		
		deque<BrickType> ShuffleBag = BrickTemplate::GetShuffleBag();
		std::random_device deviceSeed;
		std::mt19937 rdEngine(deviceSeed());
 
		shuffle(ShuffleBag.begin(), ShuffleBag.end(), rdEngine);
		BrickBag.insert(BrickBag.end(), ShuffleBag.begin(), ShuffleBag.end());
	}
	const BrickType Res = BrickBag.front();
	BrickBag.pop_front();
	return Res;
}
