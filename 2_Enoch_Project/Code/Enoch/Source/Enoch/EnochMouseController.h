// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>

#include "Enoch.h"
#include "Common/Commander.h"
#include "GameFramework/PlayerController.h"
#include "EnochMouseController.generated.h"

/**
 * 마우스를 화면에 표시하고, 관련 이벤트를 처리하기 위해 만든 컨트롤러
 */
UCLASS()
class ENOCH_API AEnochMouseController : public APlayerController
{
	GENERATED_BODY()
public :
	/** 드래그 캔슬 시, 드롭 용병 스폰. widget to field. 필드 내부 이동도 widget 거치기 때문에 사용함. 델리게이트 바인딩으로 사용*/
	UFUNCTION(BlueprintCallable)
	void DragDropSpawnFL(const FVector2D& ScreenVec2D);
	
	/** 스크린 좌표 -> 셀 좌표로 변환용 공용 함수*/
	UFUNCTION(BlueprintCallable)
	FHitResult ScreenToCellLocation(const FVector2D& ScreenVec2D);

	FLSaveData GetCurrentFLUnderCursor(){return CurrentFLUnderCursor;}
	FVector2D GetCurrentCellUnderCursor(){return CurrentCellUnderCursor;}
protected :
	virtual void BeginPlay() override;
	virtual void PlayerTick(float DeltaTime) override;	
	
	void OnClick();
	UFUNCTION(BlueprintCallable)
	void StartFight();
	UFUNCTION(BlueprintCallable)
	void EndFight();
	void OnRightClick();

	/**현재 커서 밑에 있는 용병 정보. 드래그용*/
	FLSaveData CurrentFLUnderCursor;
	/**현재 커서 밑에 있는 용병 좌표. 드래그 용*/
	FVector2D CurrentCellUnderCursor;
	/** 용병 생성 로직.
	 * OnClick과 DragDropSpawnFl 함수 공통 로직을 담고 있음
	 * @param InHitResult 마우스 히트 정보. 해당 정보로 스폰함.
	 * @author KimYubin */
	void FreeLancerSpawn(const FHitResult& InHitResult);
	/** Friendly, Enemy <p>적과 아군 숫자 둘다 있을때만 실행 가능*/
	std::pair<int,int> FriendEnemyCount;
private:
	UPROPERTY()
	class AEnochFight* fightSimulator;
	/** 직전 선택한 필드용병 시리얼 넘버 저장. 틱마다 업데이트 되는 현상 제거*/
	UPROPERTY()
	int PrevFLSerialNumber;
	/** 직전 선택한 용병 저장. 새로운 용병이 선택되면 직전 용병은 드래그 디텍터 끄는 용도.*/
	UPROPERTY()
	class AEnochFreeLancer* APrevFreeLancer;
};
