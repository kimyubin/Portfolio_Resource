// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <memory>
#include "EnochFieldData.h"
#include "EnochFreeLancerState.h"
#include "FLActs/EnochFreeLancerAct.h"
#include "SkillData.h"
#include <string>
#include <array>
#include <vector>
#include <list>

typedef uint8_t FLTEMPLATE_ID;
class FreeLancerTemplateID {
public:
	const static FLTEMPLATE_ID None = 0;
	const static FLTEMPLATE_ID ALLY_DEFAULT = 1;
	const static FLTEMPLATE_ID ENEMY_DEFAULT = 2;
	static uint8 Num;
};

typedef uint8_t ALLIANCE_ID;
class AllianceID {
public:
	const static ALLIANCE_ID None = 0;
	static ALLIANCE_ID Num;
};

enum class AllianceScope : uint8_t
{
	Alliance,
	AllyAll,
	EnemyAll,
};

UENUM()
enum class AIType : uint8 {
	Meele = 0,
	Range = 1,
	Assassin = 2, 
    Num,
};

// 서버/클라이언트 공용 구조체 모음

/** 용병 저장용 데이터 페어.*/
struct FLSaveData 
{
public:
	FLSaveData()
	{
		FLTemplateID = FreeLancerTemplateID::None;
		FLLevel = 0;
	}
	FLSaveData(uint8 FLTID, uint8 Level)	
	{
		FLTemplateID = FLTID;
		FLLevel = Level;
	}	
	FLTEMPLATE_ID FLTemplateID;
	uint8 FLLevel;		// 1 ~ 5 레벨, 0 은 없음
	//합성에 필요한 데이터만 비교. 아이템 등은 비교X
	inline bool operator == (const FLSaveData& rFL) { return ((FLTemplateID == rFL.FLTemplateID) && (FLLevel == rFL.FLLevel));}

	//그외 특성, 아이템 추가예정.	
};

/** 용병 합성 목록 구조체<br>
* CanCombine - bool 합성할 수 있는 용병 여부<br>
* InvenList - vector<int> 인벤 번호 목록<br>
* FieldList - vector<Vector2D> 필드 좌표 목록
*/
struct FreeLancerCombineData
{
	FreeLancerCombineData()
	{
		CanCombine = false;
		InvenList = vector<int>();
		FieldList = vector<Vector2D>();
		FinalCombineFL = FLSaveData();
	}
		
	bool CanCombine;
	/**합성에 사용될 예정인 인벤 용병 목록*/
	vector<int> InvenList;				
	/** 합성에 사용될 예정인 필드 용병 목록*/
	vector<Vector2D> FieldList;
	/**최종 합성 형태*/
	FLSaveData FinalCombineFL;	
};
/**
 * 적풀 선택 상태 저장용 구조체
 */
struct EnemyPoolSelectionState
{
	EnemyPoolSelectionState()
	{
		SelectedPool = -1;
		SelectedEnemyGroup = -1;
		FinalFightEnemy = -1;
	}
	EnemyPoolSelectionState(int InSelectedPool, int InSelectedEnemyGroup, int InFinalFightEnemy)
	{
		SelectedPool = InSelectedPool;
		SelectedEnemyGroup = InSelectedEnemyGroup;
		FinalFightEnemy = InFinalFightEnemy;
	}
	/** 현재 선택된 적풀. 미선택시 -1*/
	int8 SelectedPool;
	/** 적풀 내부에 있는 3개 적 그룹 중 선택된 그룹. 미선택시 -1*/
	int8 SelectedEnemyGroup;
	/** 최종적으로 싸우게 될 적*/
	int8 FinalFightEnemy;
};

class FreeLancerLevelTemplate
{
public:
	FreeLancerLevelTemplate();
	
	uint16 hp;
	float hpGen;
	
	uint16 mp;
	float mpGen;
	
	uint16 atkDamage;
	uint16 dmgMin;
	uint16 dmgMax;
	uint16 attackSpeed;
	
	uint16 armor;		// physical Damage Defence, Armor
	uint16 magicArmor;	// Magical Damage Defence, Magic resistance

	uint16 skill;
};

class FreeLancerTemplate
{
public:
	enum
	{
		LEVEL_MIN = 1,
		LEVEL_MAX = 5,
    };

	FreeLancerTemplate();
	
	FLTEMPLATE_ID ID;
	wstring name;
	uint8 grade;
	uint8 modelID;
	std::vector<ALLIANCE_ID> alliance;
	AIType AiType;
	
	uint16 attackRange;
	bool isMelee() { return attackRange > 1; }
	float attackDelayBefore;
	float attackDelayAfter;
	float shotSpeed;
	int moveSpeed;
	uint8_t buyPrice;
	
	std::array<FreeLancerLevelTemplate, LEVEL_MAX> levelData;

	static const FreeLancerTemplate* GetFreeLancerTemplate(FLTEMPLATE_ID ID);
	//TID기반 ModelID 검색
	static const uint8 GetFreeLancerModelID(uint8 TID);
	static bool InitFreeLancerTemplate(wstring path);
	static void UninitFreeLancerTemplate();
};

/**
 *  FreeLancer에 대한 정보를 가지고 있는 일반클래스
 * 이 클래스에서 인게임클래스를 수정하지 않음
 */

class FLBattleData
{
public:
	FLBattleData(int SerialNumberIn) : 
		templateID(FreeLancerTemplateID::None), SerialNumber(SerialNumberIn) {};
	~FLBattleData();
	void BeginPlay();
	void EndPlay();
	
	Vector2D location;
	bool isEnemy;
	bool expired;

public:
	FreeLancerState GetState();
	void SetState(FreeLancerState stateIn);	//수행 동작 변경
public :
	//class FreeLancerAct act;	//현재 동작
	void Act(float deltaTime);
	unique_ptr<vector<shared_ptr<FreeLancerAct>>> actMap;
	FreeLancerState state;	//현재 상태 (수행중인 동작)
	FLTEMPLATE_ID GetTID() { return templateID; }; // 용병 정보 업데이트용
	uint8 GetLevel() { return level; }
private :
	FLTEMPLATE_ID templateID;	//프리랜서 종류
	uint8 level;	// 0 ~ 4 레벨

public:
	Vector2D direction;			//바라보고 있는 방향
	float actDelay = 0;
	class FLMove &GetMove();
	class FLMove& GetJump();
	float stateDuration;	//현재 수행하고 있는 모션의 점유 예정시간(남은시간이 아닌 전체시간)
	bool CanUseSpell();	//스킬을 쓸 수 있는 상태인가?
	bool CanAttack();
	void UpdateTime(float deltaTime);	// 다음 공격, 버프, 기타등등 시간 업뎃
	void Release();
	Vector2D locationOnFight;	//현재 위치(전투중)
	queue<shared_ptr<class EnochProjectileData>> ProjectileAvailable;	//잔여 탄환이지만, 바꿀 예정이므로 사용 금지

	//전투 스테이터스 관련
	void InitStatus(uint8 TID, uint8 level, int SN, bool isEnemy);	//스테이터스 초기화
	static void DoAttack(class FLBattleData* attacker, class FLBattleData* defender); //실제 타격시점에 호출되는 함수(근접유닛)
	static void DoAttack(class EnochProjectileData* attacker, class FLBattleData* defender); //실제 타격시점에 호출되는 함수(원거리유닛) -> 추후 위 함수와 통합예정. 사용금지
	static float CalculateDamage(int attackDamage, class FLBattleData* defender, bool isPhysicalAtk = true);	//데미지 계산 및 적용 함수. isPhysicalAtk로 물공 마공 판단. 현재는 기본으로 물공으로 판단. 리턴

	int hpMax;	//최대체력
	float hpNow;	//현재체력
	float hpRegeneration;	//체력회복속도

	int mpMax;	//최대마나
	float mpNow;	//현재마나
	float mpRegeneration;	//마나회복속도

	int attackDamage;	//공격력
	int attackSpeed;	//공격속도
	int attackRange;	//사정거리
	
	bool isMelee;	//true일경우 근접유닛, false일경우 원거리유닛
	int defense;	//방어력
	float physicalReduce;	// 방어력에 따른 받는 물리 데미지 비율
	float magicRegist;		// 마법저항력에 따른 받는 마법 데미지 비율

	uint16 skill;

	float timeForOneAttack;
	float nextAttackTime;
	bool canJump;

	// 스탯 업데이트 관련
	enum StatFlag : uint32_t {
		ALL		= (unsigned)-1,
		HPMAX	= 1 << 1,
		MPMAX	= 1 << 2,
		HPGEN	= 1 << 3,
		MPGEN	= 1 << 4,
		ATKDMG	= 1 << 5,
		ATKSPD	= 1 << 6,
		ATKRNG	= 1 << 7,
		DEF		= 1 << 8,
		MAGREG	= 1 << 9,
	};

	void UpdateBaseStat(); //현재 affectedList에 들어있는 스텟 증감정보 모두 적용
	void UpdateStat(StatFlag flag = StatFlag::ALL);	//빈 함수. 사용금지

	array<list<AffectedSkill>, (int)AffectedSituation::Count> affectedList;

public: //시뮬레이션 관련
	void BeginSimulate();
	
	Circle getCollidCircle();
public:
	inline int GetSerialNumber() { return SerialNumber; };

public:
	inline FLBattleData* GetTarget();
	inline void SetTarget(int);
	inline void SetTarget(FLBattleData*);
private:
	int _target;	//현재 어그로 대상의 시리얼 넘버
	int SerialNumber; //프리랜서간 구분용 시리얼넘버. 수정하지 말 것

private:
	void processOnHit(class FLBattleData* defender);
};

struct AllianceTemplate
{
public:
	ALLIANCE_ID ID;
	wstring name;
	uint8 maxLevel;
	SKILL_ID skillID;
	vector <pair<uint8_t, AllianceScope>> levelData;	// 최소 구성 인원, 영향 범위

	static bool InitAllianceTemplate(wstring path);
	static const AllianceTemplate* GetAllianceTemplate(ALLIANCE_ID ID);
	static void UninitAllianceTemplate();
};
