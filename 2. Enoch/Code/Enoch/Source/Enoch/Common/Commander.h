#pragma once
#include <vector>
#include "EnochFreeLancerData.h"
#include "EnochCommonEnums.h"
#include "Engine/DataTable.h"

/** 서버/클라이언트 공용 커맨더
 * DB에 저장되거나 서버에서 필요한 함수만 작성
 */
class EnochCommander
{
public:
	EnochCommander();
	
	uint8 abilityPoint;	// 특성 포인트
	uint16 gold;		// 골드, 돈
	uint8 level;		// 레벨
	uint16 exp;			// 경험치

	uint16 floor;		// 현재 층
	uint8 life;			// 남은 라이프
	uint16 countWin;	// 승리 수
	uint16 countLose;	// 패배 수
	/** 현재 게임 진행 상황. 풀 선택 화면인지 전투 준비 화면인지*/
	EGameProgress CurrentProgress;
	/**현재 필드에 스폰된 용병 수*/
	uint16 fieldFreeLancerCount; 	
	
	uint32 addExp(uint32 add);

	static uint16 getExpForNextLevel( uint8 level );
	static uint16 getMaxFreeLancerNumber( uint8 level );
	
	/** 상점 슬롯 최대 개수*/
	static constexpr uint8 MAX_RECRUITSLOT = 10;
	/** 인벤 슬롯 최대 개수*/
	static constexpr uint8 MAX_INVENSLOT = 20;
	/** 합성 조건 용병수. 합성되는 용병 - 1*/
	static constexpr uint8 COMBINE_NUM = 1;
	/** 현재 상점 리스트 */
	vector<FLSaveData> RecruitFreeLancerList;
	/** 합성 가능한 영웅 목록*/
	vector<FreeLancerCombineData> CombinePossibleList;
	/** 현재 인벤 리스트 */
	vector<FLSaveData> InvenFreeLancerList;
	/** 필드 저장용 */
	vector<vector<FLSaveData>> FieldFreeLancerList;
	/** 아군/적군 반절 임시 저장용 */
	vector<vector<FLSaveData>> HalfFieldList;
	/** 적 풀 리스트. 싸울 예정인 적. 풀 3개 * 9명 * 적 필드(x * y) */
	vector<vector<vector<vector<FLSaveData>>>> EnemyPoolFieldList;
	/** 각 적풀의 등급별 용병 분류.
	 * 적풀 3개 * 등급 5개 * 각 용병 수 * TID, count*/
	vector<vector<vector<pair<uint8, int>>>> EnemyPoolFLbyGradeList;
	/** 적풀 선택 상태 저장용 구조체 */
	EnemyPoolSelectionState EnemyPoolSelectInfo;
	
	//Methods
	/**
	 * 용병 구매 공용로직. 구매 금액 및 상점 슬롯이 유효한지 검사.
	 * @param RecruitListIdx 상점 리스트 번호
	 * @return first-실패 사유, second-소요되는 재화
	 */
	std::pair<FailReason, int> BuyFreeLancer(const int& RecruitListIdx);
	/** 상점->인벤 구매
	 * @param RecruitListIdx 상점 아이템 인덱스, @param InvenListIdx 넣고 싶은 인벤 인덱스
	 * @return 구매 성공시 true, 돈이나 인벤 리스트에 자리가 없으면 false
	 */
	FailReason BuyFLToInven(const int& RecruitListIdx, const int& InvenListIdx);
	
	/** 상점->필드 구매 및 용병 배치<br>구매로직 포함.
	 * @param RecruitIdx 상점 슬롯 번호
	 * @param FieldCell 용병 위치
	 * @return 구매 성공시 true, 돈이나 필드에 자리가 없으면 false
	 */
	FailReason BuyDisposeFLToField(const int& RecruitIdx, const Vector2D& FieldCell);
	
	/** 필드->인벤 용병 회수
	 * @param FieldCell 용병 위치
	 * @param InvenIdx 인벤 슬롯 번호
	 */
	void BringFLFieldToInven(const Vector2D& FieldCell, const int& InvenIdx);

	/** 인벤->필드 용병 배치
	 * @param FieldCell 용병 위치
	 * @param InvenIdx 인벤 슬롯 번호
	 */
	FailReason DisposeFLInvenToField(const Vector2D& FieldCell, const int& InvenIdx);
	
	/** 용병 판매 금액 계산기. 판매 예상액 보여주는 용도로도 쓸예정이라 로직 분리 */
	static const uint8_t FLSellPriceCalc(const FLSaveData& FL);
	/** 인벤 용병 판매
	 * @param InvenIdx 판매를 원하는 인벤 슬롯 번호.
	 */
	void SellInvenFreelancer(const int& InvenIdx);
	/** 필드 리스트 내부 원소 위치 교환*/
	void MoveFLInsideField(const Vector2D& AVec2D,const Vector2D& BVec2D);
	/** 인벤 리스트 내부 원소간 위치 교환*/
	void SwapInvenSlot(const int& Aidx, const int& Bidx);
	
	/**
	 * 상점 마우스 오버 혹은 드래그 시 합성 가능한 용병 탐색.
	 * @return 0 - bool 합성할 수 있는 용병 여부<br>
	 * 1 - vector<int> 인벤 번호 목록<br>
	 * 2 - vector<Vector2D> 필드 좌표 목록
	 */
	FreeLancerCombineData FindFLCanCombine(FLSaveData FindFL);
	/**용병 합성을 위해 사용되는 용병들을 필드, 인벤 리스트에서 제거
	 * 구매 직후 작동
	 * @param CombineIdx 합성가능한 목록
	 */
	bool CombineDelFL(const FreeLancerCombineData& CombineIdx);	
	/** 구매할 경우 합성이 가능한 용병을 찾고, 구매 리스트의 레벨을 바꿈
	 * 합성에 사용되는 용병을 인덱스로 보유하기 때문에, 인벤/필드 내부에서 이동 시, 인덱스 업데이트를 위해 재탐색 해야함.
	 */
	void CombineRecruitList();

	/** 필드 배치 가능 인원수 초과되는지 확인.
	* @return 현재 배치된 count가 이미 최대 수치(=추가배치 불가능)이면 false.
	*/
	bool IsValidFieldCount();
	//필드 복제 기능들 
	void FriendToHalfList();
	void HalfToEnemyList();
	void HalfToFriendList();
	
	/**
	 *	레벨 별 용병 모집시 등급
	 *	@param level : 현재 레벨
	 *	@param probList : 등급별 출현 확률<p>( 인덱스 : 등급 - 1 )</p>
	 *	@return : 전체 풀의 합
	 */
	static uint32 getRecruitProb( uint8 level, std::vector<uint32> &probList );
	
	enum {
		DEFAULT_LIFE = 5,
		LEVEL_MAX = 10,
	};
	
};
