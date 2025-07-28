// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <bitset>

#include "CoreMinimal.h"
#include "BrickTemplate.h"
#include "GameFramework/Actor.h"
#include "EMDropBrick.generated.h"

UCLASS()
class EVERYMINO_API AEMDropBrick : public AActor
{
	GENERATED_BODY()
	
public:	
	AEMDropBrick();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	void InitBrick(class ABrickBoardManager* InOwnerBoardManager, EDropStartDirection InStartDirection);
	
	/** 초기 위치로 이동 후 새로운 브릭으로 재구성*/
	void NewBrick();
	
	/**
	 * DropBrick정보를 바탕으로 DroppingUnits을 업데이트함.
	 * 변경점이 있는 경우만 업데이트 그외에는 기존 유닛 사용
	 */
	void UpdateDropBrick();

	/** 인풋 정보 취합 후 행동 결정*/
	void PassInput(bitset<32> InPressInputBS);
	
	/** 데이터 검증 후 Brick유닛 위치까지 변경.*/
	void SpinDropBrick(EMInput InInput);	

	/** DropBrickData 회전 */
	void SpinBrickData(EMInput SpinInput);
	
	/**
	 * 베이스 포인트만 이동
	 * @return 이동 성공하면 이동 후 true, 실패하면 이동하지 않고 false
	 */
	bool MoveBasePoint(EMInput InInput);
	
	/** DropBrickData 이동 좌,우만 제어	 */
	void ShiftDropBrick(EMInput InInput);

	/** 호출 시 각 브릭의 시작 위치에 따른 소프트 드롭 방향으로 1칸 이동*/
	void MoveSoftDrop();
	
	void MoveHardDrop();

	/** 지정된 위치로 유닛 이동*/
	void MoveBrickUnits();

	/** 예상 투하 위치 표시*/
	void DrawGhostBrick();

	/**
	 * 락다운 체크하고, 락다운이면 게임판에 부착 후 새 브릭 생성
	 * @return 락다운이면 true
	 */
	bool LockDownCheck();
	
	bool CheckBoard();

	/** 현재 선택되어 조작 중인 브릭인지 판단*/
	void SetIsSelected(bool InSelected);
	/** 이동 버튼 뗄 때 발동*/
	void SetMoveButtonRelease();
	
	/** 인덱스 값에 대한 스킨 정보*/
	UnitSkin GetUnitSkin(int InX, int InY) const;

	/** 원점 대비 유닛의 상대좌표.*/
	FVector2I GetUnitLocation(int InX, int InY) const;

	/** 현재 필드의 게임 오버 여부. true면 해당 필드 진행 중단.*/
	bool GetFieldGameOver() const { return FieldGameOver; }

	/** 현재 필드 게임 오버로 판단. 호출시 FieldGameOver=true 처리하고, 추가 로직 실행. 외부에서 false처리 안됨.*/
	void SetFieldGameOver();

	//Variables
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TransformComponent;
	
	UPROPERTY()
	class ABrickBoardManager* OwnerBoardManager;

	
	//드롭 브릭의 구성요소에 관한 데이터
	EDropStartDirection MyStartDirection;	//현재 브릭이 떨어지는 곳. 사분면.	
	FVector2I BasePoint;					//현재 브릭 위치의 기준이 되는 좌표. 브릭의 좌상단 좌표.
	BrickType MyBrickType;					//현재 브릭 모양
	BrickDirection MyBrickDirection;		//현재 브릭 방향
	vector<vector<UnitSkin>> Brick;			//현재 브릭을 구성하는 각 유닛의 위치 및 모양 데이터 구성

	/** DropBrick을 구성하는 실제 유닛 */
	vector<vector<class AEMUnitBrick*>> DropBrickUnits;

private:
	bool IsSelected;						//현재 선택되어 조작 중인 브릭인지 판단

	bool FieldGameOver;						// 현재 필드의 게임 오버 여부. true면 해당 필드 진행 중단.
	
	float AutoFallingCumulativeTime;		//자동 낙하 측정용 누적 시간	
	float SoftDropFactorCumulativeTime;		//소프트 드랍 간격 측정용 누적 시간
	float DelayedAutoShiftCumulativeTime;	//자동 이동 대기 측정용 누적 시간. 버튼 누른시간
	float AutoRepeatRateCumulativeTime;		//자동 이동 간격  측정용 누적 시간
	bool IsDasFirstMove;					//DAS 측정시 최초 1회는 즉각 반응하기 위함. 최초 입력에서 true를 유지하기 위해, IsSelected가 false가 될 때 true로 초기화. 이동 버튼 뗄때 true로 초기화.
};
