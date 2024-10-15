// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BrickBoardManager.h"

#include <random>

#include "EMDropBrick.h"
#include "EMGameInstance.h"
#include "EMGamePlayStatics.h"
#include "EMPlayData.h"
#include "EMUnitBrick.h"

#include "Everymino.h"

ABrickBoardManager::ABrickBoardManager()
{
	PrimaryActorTick.bCanEverTick = true;
	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComp"));
	RootComponent = TransformComponent;
	HoldBrickType = BrickType::None;
	PressInputBS.reset();
	CurrentControlZone = EDropStartDirection::North;
}

// Called when the game starts or when spawned
void ABrickBoardManager::BeginPlay()
{
	Super::BeginPlay();
	
	Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->ChangeUnitSkin(BrickType::L, 3,UnitSkin::Skin);

	InitSpawnBoard();
	DropBricks = vector<AEMDropBrick*>(EnumToInt(EDropStartDirection::Size), nullptr);
	
	for (int i = 0; i < DropBricks.size(); ++i)
	{		
		DropBricks[i] = GetWorld()->SpawnActor<AEMDropBrick>(AEMDropBrick::StaticClass(),FVector::ZeroVector,FRotator::ZeroRotator);
		
		DropBricks[i]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		DropBricks[i]->InitBrick(this, static_cast<EDropStartDirection>(i));
	}
	//선택된 사분면 정보전달.
	DropBricks[EnumToInt(CurrentControlZone)]->SetIsSelected(true);
}

void ABrickBoardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//상하 좌우 이동만 제어
	if (PressInputBS.any())	
		DropBricks[EnumToInt(CurrentControlZone)]->PassInput(PressInputBS);	

	LineClearCheck();
}

void ABrickBoardManager::InitSpawnBoard()
{
	const auto EMPlayDataPtr = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData();
	const auto GBoard = EMPlayDataPtr->GetCurrentGameBoard();
	const int MaxY = EMPlayDataPtr->GetMaxBoardY();
	const int MaxX = EMPlayDataPtr->GetMaxBoardX();

	//유닛 1개가 이미 센터에 위치하기 때문에 1개 빼줌.
	SetActorLocation(FVector(- BrickTemplate::UNIT_SIZE*(MaxX-1)/2,0.f,BrickTemplate::UNIT_SIZE*(MaxY-1)/2));
	if(GameBoardData.size()==0)
		GameBoardData = GBoard;
	
	for (int y = 0; y < MaxY; ++y)
	{
		GameBoardUnits.push_back(vector<AEMUnitBrick*>());
		for (int x = 0; x < MaxX; ++x)
		{
			GameBoardUnits.back().push_back(UEMGamePlayStatics::SpawnUnit(x, y, GameBoardData[y][x], this));
		}
	}
}
void ABrickBoardManager::RebuildBoard()
{
	for(int y = 0; y < GameBoardUnits.size(); ++y)
	{
		for(int x = 0; x < GameBoardUnits[y].size();++x)
		{
			UnitSkin NextUnitSkin = GameBoardData[y][x];
			UnitSkin NowUnitSkin = UnitSkin::Empty;

			if (GameBoardUnits[y][x] != nullptr)
				NowUnitSkin = GameBoardUnits[y][x]->MySkinNum;
			
			if(NowUnitSkin == NextUnitSkin)
				continue;

			if (NowUnitSkin == UnitSkin::Empty) //&& if (NextUnitSkin != UnitSkin::Empty)
			{
				GameBoardUnits[y][x] = UEMGamePlayStatics::SpawnUnit(x, y, NextUnitSkin, this);
			}
			else // if (NowUnitSkin != UnitSkin::Empty) 
			{
				if (NextUnitSkin == UnitSkin::Empty)				
					UEMGamePlayStatics::DestroyUnit(&GameBoardUnits[y][x]);				
				else
					GameBoardUnits[y][x]->SetSkinMesh(NextUnitSkin);
			}			
		}
	}
}
bool ABrickBoardManager::CheckBoard(AEMDropBrick* InDropBrick)
{
	//각 드롭브릭이랑 검사도 추가해야함.
	const vector<vector<UnitSkin>> InBrick = InDropBrick->Brick;
	const auto EMPlayDataPtr = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData();
	
	//중앙판 벽 위치
	const uint16 CenterNorthBorder = EMPlayDataPtr->GetFrontCenterPoint();
	const uint16 CenterEastBorder = EMPlayDataPtr->GetBackCenterPoint();
	const uint16 CenterSouthBorder = EMPlayDataPtr->GetBackCenterPoint();
	const uint16 CenterWestBorder = EMPlayDataPtr->GetFrontCenterPoint();
	
	for(int dby = 0; dby < InBrick.size();++dby)
	{
		for(int dbx = 0; dbx < InBrick[dby].size();++dbx)
		{			
			//드롭 유닛이 존재하면
			if (InBrick[dby][dbx] != UnitSkin::Empty)
			{
				const FVector2I DropBrickPos = InDropBrick->GetUnitLocation(dbx,dby);
				//경계 검사.
				if (DropBrickPos.Y >= GameBoardData.size()
					|| DropBrickPos.X >= GameBoardData.front().size()
					|| DropBrickPos.Y < 0
					|| DropBrickPos.X < 0)
					return false;
				//중앙판 경계 검사
				//각 경계는 해당 사면에서 출발하는 브릭만 통과할 수 있음
				if (InDropBrick->MyStartDirection != EDropStartDirection::North
					&& DropBrickPos.Y <= CenterNorthBorder)
					return false;
				if (InDropBrick->MyStartDirection != EDropStartDirection::East
					&& DropBrickPos.X >= CenterEastBorder)
					return false;
				if (InDropBrick->MyStartDirection != EDropStartDirection::South
					&& DropBrickPos.Y >= CenterSouthBorder)
					return false;
				if (InDropBrick->MyStartDirection != EDropStartDirection::West
					&& DropBrickPos.X <= CenterWestBorder)
					return false;

				// 유닛 겹침 검사.
				if (GameBoardData[DropBrickPos.Y][DropBrickPos.X] != UnitSkin::Empty)
					return false;
			}
		}
	}
	return true;
}

void ABrickBoardManager::AttachDropBrick(AEMDropBrick* InDropBrick)
{
	if(InDropBrick->GetFieldGameOver())
		return;

	//필드 오버여도 일단 현재 드롭 브릭을 게임판에 모두 부착한 뒤 실행하기 위한 용도
	bool FieldOver = false;

	const auto EMPlayDataPtr = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData();
	const uint16 VanishZone = EMPlayDataPtr->GetVanishZone();
	const uint16 TotalLength = EMPlayDataPtr->GetMaxBoardX();
	
	for(int dby = 0; dby < InDropBrick->Brick.size();++dby)
	{
		for(int dbx = 0; dbx < InDropBrick->Brick[dby].size();++dbx)
		{			
			if (InDropBrick->GetUnitSkin(dbx, dby) != UnitSkin::Empty)
			{
				const FVector2I DropBrickPos = InDropBrick->GetUnitLocation(dbx,dby);
				
				switch (InDropBrick->MyStartDirection)
				{
				case EDropStartDirection::North:
					if (DropBrickPos.Y < VanishZone)
						FieldOver = true;
					break;
				case EDropStartDirection::East:
					if (DropBrickPos.X > TotalLength - VanishZone)
						FieldOver = true;
					break;
				case EDropStartDirection::South:
					if (DropBrickPos.Y > TotalLength - VanishZone)
						FieldOver = true;
					break;
				case EDropStartDirection::West:
					if (DropBrickPos.X < VanishZone)
						FieldOver = true;
					break;
				default:
					break;
				}
				
				GameBoardData[DropBrickPos.Y][DropBrickPos.X] = InDropBrick->Brick[dby][dbx];
				GameBoardUnits[DropBrickPos.Y][DropBrickPos.X] = InDropBrick->DropBrickUnits[dby][dbx];
				GameBoardUnits[DropBrickPos.Y][DropBrickPos.X]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				InDropBrick->DropBrickUnits[dby][dbx] = nullptr;
			}
		}
	}
	if(FieldOver)
	{
		InDropBrick->SetFieldGameOver();
	}
	else
		InDropBrick->NewBrick();
}

pair<bool, int> ABrickBoardManager::LineClearCheck()
{
	const auto EMPlayDataPtr = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData();

	int LastClearFloor = 0;
	//todo 1. 각축 별로 라인 클리어 체크
	//todo 2. 중심 축
	//천장부터 바닥 직전까지 훑음.
	
	for(int gby = 0; gby < GameBoardData.size()-1; ++gby)
	{
		auto it = find(GameBoardData[gby].begin(), GameBoardData[gby].end(), UnitSkin::Empty);
		//한줄 전체에 빈공간이 없음. == 꽉참
		if (it == GameBoardData[gby].end())
		{
			//클리어층 바로 위칸부터 한칸씩 내리기. 0번은 천장이라 따로 계산
			for (int cgby = gby; cgby > LastClearFloor; --cgby)
			{
				//좌우 벽만 빼고 복사. 벽 스킨 때문에 통으로 내리는게 아니라 중간만 복사
				for (int x = 1; x < GameBoardData[cgby].size() - 1; ++x)
					GameBoardData[cgby][x] = GameBoardData[cgby - 1][x];
			}
			if(LastClearFloor == 0)
				GameBoardData[0] = EMPlayDataPtr->GetCurrentGameBoard()[0];

			//이미 내려서 확정적으로 빈칸인 곳 바로 아래는 위칸이 빈칸이므로
			//그냥 기본 정보를 복사해서 가져옴
			//ex) 1줄 삭제 -> 전체 1칸 내리니까, 0번(맨위)은 무조건 빈칸 -> 0번은 복사
			//ex) 2줄 삭제 -> 전체 2칸 내리니까, 0번, 1번은 빈칸 확정 + 0번은 이미 빈칸 -> 1번만 복사
			GameBoardData[LastClearFloor] = EMPlayDataPtr->GetCurrentGameBoard()[LastClearFloor];
			LastClearFloor++;
		}		
	}

	if(LastClearFloor > 0)
	{
		RebuildBoard();
		return pair<bool, int>(true, LastClearFloor);		
	}
	
	return pair<bool, int>(false, 0);
	
}

void ABrickBoardManager::SetPressInput(EMInput InInput)
{
	PressInputBS.set(EnumToInt(InInput));
}

void ABrickBoardManager::SetReleasedInput(EMInput InInput)
{
	PressInputBS.reset(EnumToInt(InInput));
	
	switch (InInput)
	{
	case EMInput::LeftMove:
	case EMInput::RightMove:
	case EMInput::UpMove:
	case EMInput::DownMove:
		DropBricks[EnumToInt(CurrentControlZone)]->SetMoveButtonRelease();
		break;
	default:break;
	}	
}

void ABrickBoardManager::PressedRotateInput(EMInput InInput)
{
	DropBricks[EnumToInt(CurrentControlZone)]->SpinDropBrick(InInput);
}
void ABrickBoardManager::PressedHardDrop()
{
	DropBricks[EnumToInt(CurrentControlZone)]->MoveHardDrop();
}

void ABrickBoardManager::SetCurrentControlZone(EDropStartDirection InZone)
{
	//선택된 사분면 변경 및 해당 브릭에 정보 전달.
	DropBricks[EnumToInt(CurrentControlZone)]->SetIsSelected(false);
	CurrentControlZone = InZone;
	DropBricks[EnumToInt(CurrentControlZone)]->SetIsSelected(true);
}

