#pragma once

// 서버/클라이언트 공용 enum class 모음

/** 구매, 배치 등의 실패 사유*/
enum class FailReason : uint8 {
	Success = 0,
	None = 1,
	NotEnoughGolds = 2,
	FieldAlreadyFull = 3,
	InvenSlotIsFull = 4,
	RecruitSlotISFull = 5,
    
};
/**커맨더 별 게임 진행 현황.*/
UENUM()
enum class EGameProgress : uint8
{
	None = 0,
	SelectEnemyPool = 1,
	BattlePreparation = 2,	
};
