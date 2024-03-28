// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Common/Commander.h"
#include "UIs/PlayMenuUIManager.h"

#include "CommanderWrapper.generated.h"


/** 클라이언트 전용 커맨더
 * 클라이언트에서만 필요한 함수 작성
 * EnochCommander 에서 블루프린트에 필요한 함수/데이터 가져오기(래핑)
 */
UCLASS(Blueprintable)
class ENOCH_API UCommanderWrapper : public UObject
{
	GENERATED_BODY()

private:
	// 커맨더 원본 정보
	EnochCommander commander;
public:
	UCommanderWrapper();

	void InitCommander();
	void SetCommander(EnochCommander* _commander);

	UFUNCTION(BlueprintCallable)
	uint8 getAbilityPoint() { return commander.abilityPoint; }
	UFUNCTION(BlueprintCallable)
	uint8 addAbilityPoint(int add);

	UFUNCTION(BlueprintCallable)
	int32 getGold() { return commander.gold; }
	UFUNCTION(BlueprintCallable)
	int32 addGold(int add);

	UFUNCTION(BlueprintCallable)
	uint8 getLevel() { return commander.level; }
	UFUNCTION(BlueprintCallable)
    FString GetLevelString();

	UFUNCTION(BlueprintCallable)
	uint8 getExp() { return commander.exp; }
	UFUNCTION(BlueprintCallable)
	uint8 addExp(int add);
	UFUNCTION(BlueprintCallable)
    FString GetExpString();
	
	
	
	/** 메인 UI 접근용. 게임인스턴스 접근 제한으로인해 넣었음. 게임인스턴스에 있는 MyPlayMenuUI와 동일.*/
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UPlayMenuUIManager* CommanderPlayMenuUI;
	/**
	 * 필드에 스폰된 프리랜서 리스트 업데이트
	 * @param CellLocation 셀 위치
	 * @param FreeLancerVar 저장용 데이터 페어(FTID, Lv)
	 */
	void SetFieldFreeLancerList(const FVector2D &CellLocation, const FLSaveData &FreeLancerVar);
	/**
	 * @param X width, @param Y height;
	 * @return 필드 리스트 값.FLSaveData */
	FLSaveData GetFieldFreeLancerList(const int &X, const int &Y);
	FLSaveData GetFieldFreeLancerList(const FVector2D &CellLocation);

	//저장용 함수. 저장된 절반 필드 가져옴
	vector<vector<FLSaveData>> GetHalfField(){return commander.HalfFieldList;}
	void SetHalfField(vector<vector<FLSaveData>> LoadData){commander.HalfFieldList=LoadData;}

	//commander함수 호출용
	void FriendToHalf(){commander.FriendToHalfList();}
	void HalfToEnemy(){commander.HalfToEnemyList();}
	void HalfToFriend(){commander.HalfToFriendList();}
	/**
	 * 상점 리스트 원소 가져오기
	 * @param Idx 가져오고 싶은 원소 번호
	 * @return FLSaveData 구조체  
	 */
	FLSaveData GetRecruitListElement(const int &Idx);
	/**
	 * 인벤 리스트 원소 가져오기
	 * @param Idx 원소 번호
	 * @return FLSaveData 구조체  
	 */
	FLSaveData GetInvenListElement(const int &Idx);
	/**
	 * 상점 리스트 원소 값 변경
	 * @param Idx 원소 인덱스
	 * @param val input data (FreelancerTID, FreeLancerLv) 
	 */
	void SetRecruitListElement(const int &Idx, const FLSaveData &val);
	/**
	 * 인벤 리스트 원소 값 변경
	 * @param Idx 원소 인덱스
	 * @param val FLSaveData 해당 원소에 넣을 값
	 */
	void SetInvenListElement(const int &Idx, const FLSaveData &val);
	
	/** 인벤 리스트 내부 원소간 교환. 커맨더 내부 함수로 전달*/
	void SwapInvenSlot(const int& Aidx, const int& Bidx);

	/** 필드에서 인벤으로 용병 회수. Detach 포함.
	 * @param FieldCell 용병 위치
	 * @param InvenIdx 인벤 슬롯 번호 	 */
	void BringFLFieldToInvenWrap(const FVector2D& FieldCell, const int& InvenIdx);
	/** 인벤에서 필드로 용병 배치. 스폰 포함.
	 * @param FieldCell 용병 위치
	 * @param InvenIdx 인벤 슬롯 번호
	 * @return 스폰 성공 여부*/
	bool DisposeFLInvenToFieldWrap(const FVector2D& FieldCell, const int& InvenIdx);
	/**
	* 구매함수 Commander/BuyFreelancer()함수 반환
	* @param RecruitListIdx 구매하고 싶은 상점 아이템 인덱스, @param InvenListIdx 넣고 싶은 인벤 인덱스
	* @return 구매 성공시 true, 돈이나 인벤 리스트에 자리가 없으면 false
	*/
	bool BuyFLToInvenWrap(const int &RecruitListIdx, const int &InvenListIdx);
	/** 상점에서 필드로 구매 후 용병 배치. 스폰 포함.
	 * @param RecruitIdx 상점 슬롯 번호
	 * @param FieldCell 용병 위치
	 * @return 구매, 스폰 성공 여부
	 */	
	bool BuyDisposeFLToFieldWrap(const int& RecruitIdx, const FVector2D& FieldCell);
	/**인벤 용병 판매.
	 * @param InvenIdx 판매원하는 슬롯 번호.
	 */
	void BuyAndCombine(const int& RecruitIdx);
	void SellInvenFLWrap(const int &InvenIdx);
	/**
	 * 필드 용병 위치 이동 및 자리 맞교환<br>
	 * 빈칸이면 이동, 용병있으면 스왑<br>
	 * 내부에서 액터 제거 및 스폰까지 처리
	 * @param SelectCellVec2D 이전에 선택된 용병 위치 @param TargetCellVec2D 다음에 선택된 용병 위치
	 */
	void MoveFLInsideFieldWrap(const FVector2D& SelectCellVec2D,const FVector2D& TargetCellVec2D);

	/**	
	 * 게임 시작 시, 유저 정보 요청 및 초기화 <br>
	 * 현재 recruitlist만 생성 <br>
	 * @return 요청 및 갱신 성공시 true
	 */	
	bool ReqUserInfo();	
	/**
	 * 상점 리스트 및 적 필드 초기화 역할
	 * @return 요청 및 갱신 성공시 true
	 */
	bool ReqNewRecruitList();
	/**
	 * 서버에 적 풀 요청해서 받아오는 역할<br>
	 * 현재는 랜덤으로 채움
	 * @param IsReroll true면 리롤이라서 돈 차감함. false면 그냥 새로 받아오는 걸로 판단
	 */
	UFUNCTION(BlueprintCallable)
	bool ReqNewEnemyPool(bool IsReroll = false);
	
	/**
	 * 적 풀 번호를 바탕으로 전체 용병 정보를 정리해서 리턴
	 * @param PoolNum 적풀 패널 번호 
	 * @param OutFLCount 내부에 저장된 풀 리스트를 정리해서 리턴
	 */
	void GetEnemyPoolFLCount(int PoolNum, vector<pair<uint8,int>>& OutFLCount);

	/**
	 * 적풀 패널에서 정리한 등급별 용병 숫자 정보를 취합
	 * @param PoolNum 적풀 패널 번호
	 * @param InEnemyPoolFLbyGradeList 정리완료된 등급별 용병 숫자 정보.
	 */	
	void SetEnemyPoolFLbyGradeList(const int PoolNum,const vector<vector<pair<uint8, int>>> &InEnemyPoolFLbyGradeList);
	/**적풀 번호와 등급에 해당하는 용병 수 리턴함*/
	vector<pair<uint8, int>> GetEnemyListbyGrade(const int& PoolNum, const int&  GradeNum);
	/**
	 * 요청을 대체하는 랜덤 리스트 생성용 임시 함수<br>
	 * 현재는 근접1,2만 생성
	 * @return 임시 생성된 상점 vector<uint8,FreeLancerLv> 
	 */
	std::vector<FLSaveData> RandomRecruitListTemp();
	/** 선택된 적풀 번호를 받고, 해당 적풀에서 랜덤으로 한명 뽑아서 필드 데이터에 넣음*/
	void SetEnemyPoolSelectInfo(EnemyPoolSelectionState InEPSelet);
	void SaveGameState();
	void LoadGameState();
};
