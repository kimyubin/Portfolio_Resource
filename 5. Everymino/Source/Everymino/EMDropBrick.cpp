// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EMDropBrick.h"
#include "EMUnitBrick.h"

#include "BrickBoardManager.h"
#include "EMGameInstance.h"
#include "EMGamePlayStatics.h"
#include "EMPlayData.h"

AEMDropBrick::AEMDropBrick()
{
	PrimaryActorTick.bCanEverTick = true;
	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComp"));
	RootComponent = TransformComponent;

	OwnerBoardManager = nullptr;
	BasePoint = FVector2I();
	MyBrickType = BrickType::T;
	MyBrickDirection = BrickDirection::UP;
	Brick = vector<vector<UnitSkin>>();
	
	AutoFallingCumulativeTime = 0.f;
	SoftDropFactorCumulativeTime = 0.f;
	DelayedAutoShiftCumulativeTime = 0.f;
	AutoRepeatRateCumulativeTime = 0.f;
	IsSelected = false;
	FieldGameOver = false;
	IsDasFirstMove = true;
}

void AEMDropBrick::BeginPlay()
{
	Super::BeginPlay();
}

void AEMDropBrick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(FieldGameOver)
		return;
	
	LockDownCheck();
	
	UEMGamePlayStatics::MaxLimitAdder(SoftDropFactorCumulativeTime, DeltaTime);
	
	UEMGamePlayStatics::MaxLimitAdder(AutoFallingCumulativeTime, DeltaTime);	
	if(AutoFallingCumulativeTime >= Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetAutoFallingSpeed())
	{
		MoveSoftDrop();
		AutoFallingCumulativeTime = 0.f;
	}

	//선택된 경우에만 카운팅
	if(IsSelected)
	{
		UEMGamePlayStatics::MaxLimitAdder(DelayedAutoShiftCumulativeTime, DeltaTime);

		//대기 시간 이후부터 카운팅.
		if(DelayedAutoShiftCumulativeTime >= Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetDelayedAutoShift())
			UEMGamePlayStatics::MaxLimitAdder(AutoRepeatRateCumulativeTime, DeltaTime);
	}
}

void AEMDropBrick::InitBrick(ABrickBoardManager* InOwnerBoardManager, EDropStartDirection InStartDirection)
{
	OwnerBoardManager = InOwnerBoardManager;
	MyStartDirection = InStartDirection;
	MyBrickType = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetNewBrickType();

	switch (MyStartDirection)
	{
	case EDropStartDirection::North: MyBrickDirection = BrickDirection::UP;
		break;
	case EDropStartDirection::East: MyBrickDirection = BrickDirection::RIGHT;
		break;
	case EDropStartDirection::South: MyBrickDirection = BrickDirection::DOWN;
		break;
	case EDropStartDirection::West: MyBrickDirection = BrickDirection::LEFT;
		break;
	default: break;
	}

	BasePoint = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetDropStartPoint(MyStartDirection);
	Brick = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetBrickShape(MyBrickType, MyBrickDirection);

	//유닛 스폰 및 저장.	
	for (int dpY = 0; dpY < Brick.size(); ++dpY)
	{
		DropBrickUnits.push_back(vector<AEMUnitBrick*>());

		for (int dpX = 0; dpX < Brick[dpY].size(); ++dpX)
		{
			const UnitSkin DataSkin = GetUnitSkin(dpX, dpY);
			const FVector2I UnitLoc = GetUnitLocation(dpX, dpY);
			DropBrickUnits.back().push_back(UEMGamePlayStatics::SpawnUnit(UnitLoc, DataSkin, this));
		}
	}
}

void AEMDropBrick::NewBrick()
{
	BasePoint = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetDropStartPoint(MyStartDirection);
	MyBrickType = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetNewBrickType();

	switch (MyStartDirection)
	{
	case EDropStartDirection::North: MyBrickDirection = BrickDirection::UP;
		break;
	case EDropStartDirection::East: MyBrickDirection = BrickDirection::RIGHT;
		break;
	case EDropStartDirection::South: MyBrickDirection = BrickDirection::DOWN;
		break;
	case EDropStartDirection::West: MyBrickDirection = BrickDirection::LEFT;
		break;
	default: break;
	}
	Brick = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetBrickShape(MyBrickType, MyBrickDirection);
	UpdateDropBrick();
}

void AEMDropBrick::UpdateDropBrick()
{
	for (int dpY = 0; dpY < DropBrickUnits.size(); ++dpY)
	{
		for (int dpX = 0; dpX < DropBrickUnits[dpY].size(); ++dpX)
		{
			const UnitSkin DataSkin = GetUnitSkin(dpX, dpY);
			const FVector2I UnitLoc = GetUnitLocation(dpX, dpY);

			//실제 유닛있는데, 비어야하면 파괴.
			//있어야하면, 이동 후 스킨 변경
			if (DropBrickUnits[dpY][dpX] != nullptr)
			{
				if (DataSkin == UnitSkin::Empty)
					UEMGamePlayStatics::DestroyUnit(&DropBrickUnits[dpY][dpX]);
				else
					DropBrickUnits[dpY][dpX]->SetPositionAndSkin(UnitLoc.X, UnitLoc.Y, DataSkin);
			}
			else if (DataSkin != UnitSkin::Empty)
			{
				//실제 유닛 없는데, 데이터가 있으면 스폰
				DropBrickUnits[dpY][dpX] = UEMGamePlayStatics::SpawnUnit(UnitLoc, DataSkin, this);
			}
		}
	}
}

void AEMDropBrick::PassInput(bitset<32> InPressInputBS)
{
	if(FieldGameOver)
		return;
	
	//소프트 드랍 최소 간격마다 이동하게 제어
	if (SoftDropFactorCumulativeTime >= Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetSoftDropFactor())
	{
		SoftDropFactorCumulativeTime = 0.f;
		
		EMInput MyInside = UEMGamePlayStatics::GetInsideDirection(MyStartDirection);
		
		if (InPressInputBS.test(EnumToInt(MyInside)))
			MoveSoftDrop();
	}

	//좌우 이동만 제어
	if (InPressInputBS.test(EnumToInt(EMInput::DownMove)))
		ShiftDropBrick(EMInput::DownMove);
	if (InPressInputBS.test(EnumToInt(EMInput::LeftMove)))
		ShiftDropBrick(EMInput::LeftMove);
	if (InPressInputBS.test(EnumToInt(EMInput::UpMove)))
		ShiftDropBrick(EMInput::UpMove);
	if (InPressInputBS.test(EnumToInt(EMInput::RightMove)))
		ShiftDropBrick(EMInput::RightMove);
}

void AEMDropBrick::SpinDropBrick(EMInput InInput)
{
	if(FieldGameOver)
		return;
	
	//브릭 데이터 회전 후 검증
	SpinBrickData(InInput);

	if (CheckBoard())
	{
		UpdateDropBrick();
		return;
	}

	//회전 실패시 이동 후 검증.
	const FVector2I PrevBasePoint = BasePoint;
	for (int tryNum = 0; tryNum < 5; ++tryNum)
	{
		BasePoint += BrickTemplate::GetWallKickData(MyBrickType, MyBrickDirection, InInput, tryNum, MyStartDirection);
		if (CheckBoard())
		{
			UpdateDropBrick();
			return;
		}
		BasePoint = PrevBasePoint;
	}
	//모두 실패시 회전 원상복귀
	if(InInput==EMInput::Clockwise)
		SpinBrickData(EMInput::AntiClockwise);
	else
		SpinBrickData(EMInput::Clockwise);
}

void AEMDropBrick::SpinBrickData(EMInput SpinInput)
{
	int TempDir = EnumToInt(MyBrickDirection);
	constexpr int MaxSize = EnumToInt(BrickDirection::Size);

	if (SpinInput == EMInput::Clockwise)
		TempDir = (TempDir + 1) % MaxSize;
	else if (SpinInput == EMInput::AntiClockwise) //-1,0,1,2에 4를 더하고 나머지 연산하면 3,0,1,2
		TempDir = ((TempDir - 1) + MaxSize) % MaxSize;
	else
		return;

	MyBrickDirection = static_cast<BrickDirection>(TempDir);
	Brick = Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetBrickShape(MyBrickType, MyBrickDirection);	
}

bool AEMDropBrick::MoveBasePoint(EMInput InInput)
{
	const auto PrvBasePoint = BasePoint;

	switch (InInput)
	{
	case EMInput::UpMove:
		BasePoint.Y--;
		break;
	case EMInput::DownMove:
		BasePoint.Y++;
		break;
	case EMInput::RightMove:
		BasePoint.X++;
		break;
	case EMInput::LeftMove:
		BasePoint.X--;
		break;
	default:
		break;
	};
	
	if (!CheckBoard())
	{
		BasePoint = PrvBasePoint;
		return false;
	}
	return true;		
}

void AEMDropBrick::ShiftDropBrick(EMInput InInput)
{
	//좌우 이동만 허용 그 외엔 리턴.
	//소프트 다운 입력에서 IsDasFirstMove이 false가 되는 현상 방지하므로써,
	//소프트 다운과 좌우 이동 동시 입력 지원.
	bool IsValidInput = false;
	switch (MyStartDirection)
	{
	case EDropStartDirection::North:
		if(InInput == EMInput::LeftMove||InInput == EMInput::RightMove)
			IsValidInput = true;
		break;
	case EDropStartDirection::East:
		if(InInput == EMInput::UpMove||InInput == EMInput::DownMove)
			IsValidInput = true;
		break;
	case EDropStartDirection::South:
		if(InInput == EMInput::LeftMove||InInput == EMInput::RightMove)
			IsValidInput = true;
		break;
	case EDropStartDirection::West:
		if(InInput == EMInput::UpMove||InInput == EMInput::DownMove)
			IsValidInput = true;
		break;
	default:
		break;
	}
	
	if(!IsValidInput)
		return;
	
	//IsDasFirstMove, 최초 입력시 바로 시작하고, 대기 시간 이후에는 일정 간격으로 이동..
	if (IsDasFirstMove
		|| DelayedAutoShiftCumulativeTime >= Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetDelayedAutoShift())
	{
		if (IsDasFirstMove
			|| AutoRepeatRateCumulativeTime >= Cast<UEMGameInstance>(GetGameInstance())->GetEMPlayData()->GetAutoRepeatRate())
		{
			IsDasFirstMove = false;
			AutoRepeatRateCumulativeTime = 0.f;

			//이동 성공시에만 유닛 이동.
			if (MoveBasePoint(InInput))
				MoveBrickUnits();
		}
	}
}

void AEMDropBrick::MoveSoftDrop()
{
	const EMInput Inside = UEMGamePlayStatics::GetInsideDirection(MyStartDirection);

	if(MoveBasePoint(Inside))
		MoveBrickUnits();
}

void AEMDropBrick::MoveHardDrop()
{
	const EMInput Inside = UEMGamePlayStatics::GetInsideDirection(MyStartDirection);
	
	bool IsMoveSuccess = true;
	while (IsMoveSuccess)
	{
		IsMoveSuccess = MoveBasePoint(Inside);	
	}
	
	MoveBrickUnits();
}

void AEMDropBrick::MoveBrickUnits()
{
	for (int dpY = 0; dpY < DropBrickUnits.size(); ++dpY)
	{
		for (int dpX = 0; dpX < DropBrickUnits[dpY].size(); ++dpX)
		{
			if (DropBrickUnits[dpY][dpX] != nullptr)
			{
				const FVector2I UnitLoc = GetUnitLocation(dpX, dpY);
				DropBrickUnits[dpY][dpX]->SetPosition(UnitLoc);
			}
		}
	}
}

void AEMDropBrick::DrawGhostBrick()
{
	// auto ghostBrick = DropBrickData;
	// while(CheckBoard())
	// {
	// 	ghostBrick.BasePoint.Y++;			
	// }
	// ghostBrick.BasePoint.Y--;
	
}
bool AEMDropBrick::LockDownCheck()
{
	const EMInput Inside = UEMGamePlayStatics::GetInsideDirection(MyStartDirection);

	//베이스 포인트 하강 가능한지 확인.
	if(MoveBasePoint(Inside))
	{
		//이동 가능. 락다운 아님
		//베이스 포인트 원상복귀 
		MoveBasePoint(UEMGamePlayStatics::GetOutsideDirection(MyStartDirection));
		
		return false;
	}
	else
	{
		//이동불가. 락다운 후 드롭 브릭 붙임. 
		OwnerBoardManager->AttachDropBrick(this);

		return true;	
	}	
}

bool AEMDropBrick::CheckBoard()
{
	return OwnerBoardManager->CheckBoard(this);
}

void AEMDropBrick::SetIsSelected(bool InSelected)
{
	IsSelected = InSelected;

	//선택 변경 시 초기화.
	if(!InSelected)
		IsDasFirstMove = true;
}

void AEMDropBrick::SetMoveButtonRelease()
{
	IsDasFirstMove = true;
	DelayedAutoShiftCumulativeTime = 0.f; 
}

UnitSkin AEMDropBrick::GetUnitSkin(int InX, int InY) const
{
	return Brick[InY][InX]; 
}

FVector2I AEMDropBrick::GetUnitLocation(int InX, int InY) const
{
	return FVector2I(InX + BasePoint.X, InY + BasePoint.Y);
}

void AEMDropBrick::SetFieldGameOver()
{
	//todo 필드 오버 처리 되면 진행할 로직
	FieldGameOver = true;
}
