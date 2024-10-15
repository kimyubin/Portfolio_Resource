// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <bitset>
#include <deque>

#include "CoreMinimal.h"
#include "BrickTemplate.h"
#include "GameFramework/Actor.h"

#include "BrickBoardManager.generated.h"
using namespace std;

UCLASS()
class EVERYMINO_API ABrickBoardManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABrickBoardManager();

protected:
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;	
private:
	/** 최초 보드 스폰 */	
	void InitSpawnBoard();
	
	/** 보드 갱신 */
	void RebuildBoard();
	
public:
	/** DropBrick이 움직인 직후 유효성 검사 */
	bool CheckBoard(class AEMDropBrick* InDropBrick);
	

	/** 드롭 브릭을 게임판에 부착 후 새 브릭 소환.*/
	void AttachDropBrick(class AEMDropBrick* InDropBrick);
	
	/**
	 * 라인이 지워지는지 확인.
	 * @return first - 클리어 여부, second - 클리어한 줄 수
	 */
	pair<bool, int> LineClearCheck();
	
	/** 입력 처리*/
	void SetPressInput(EMInput InInput);
	void SetReleasedInput(EMInput InInput);
	void PressedRotateInput(EMInput InInput);
	void PressedHardDrop();
	void SetCurrentControlZone(EDropStartDirection InZone);


private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> TransformComponent;
	
	vector<vector<UnitSkin>> GameBoardData;				//게임 보드 구성 정보
	vector<vector<class AEMUnitBrick*>> GameBoardUnits;	//게임 보드를 구성하는 실제 유닛
	vector<vector<class AEMUnitBrick*>> GhostUnits;		//DropBrick의 예상 투하 지점 그림자.
	vector<class AEMDropBrick*> DropBricks;				//DropBrick 인스턴스 배열
	
	
	BrickType HoldBrickType;
	
	/** 키입력 데이터 모음 비트셋. 다중입력을 위해 비트플래그 사용 */
	bitset<32> PressInputBS;
	
	/** 현재 조작하고 있는 사면*/
	EDropStartDirection CurrentControlZone;
	
};

