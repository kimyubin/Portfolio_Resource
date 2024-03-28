#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
using namespace std;

// 스킬 액티브/패시브 효과 추가시 여기에 추가
// uint16 : 0 ~ 65535
enum class SkillEffect : uint16_t
{
	// passive
	PASSIVE_START = 0,
	HP = PASSIVE_START,
	HPGEN,
	MP,			// 최대 MP를 바꾸는 일은 없을거같지만... 일단 추가
	MPGEN,
	ATKDMG,
	ATKSPD,
	ATKRNG,
	DEF,
	MAGREG,		// 마법 저항력
	MAGDMG,		// 마법 공격력
	PUREDMG,	// 순수데미지
	MULTIEFFECT,	//인게임에서 보이면 안되는것. 엑셀에서 멀티이펙트 구분용
	SKILLRANGE,
	TARGETNUM,
	PASSIVE_END,
	SKILLWIDTH,
	STUN,
	EXPLOSIONRANGE,
	CRITICAL,
	PERCENT,
};

// 스킬 형태 : 패시브인지, 평타강화인지, 타겟팅인지...
enum class SkillShape : uint8_t
{
	Active,
	Passive,
	Target,
	Projectile,
	Num_MAX,
};

// 스킬 대상 : 아군 전체, 아군 대상, 적군 전체, ...
enum class SkillTarget : uint8_t
{
	Ally_All,
	Enemy_All,
	Enemy_Single,
	Enemy_Linear,
	Enemy_Around,
	Num_MAX,
};

// 정화 여부
enum class SkillDispel : uint8_t
{
	None,
	Num_MAX,
};

// 스킬 영향 관련 : Affected 라는 말을 공통적으로 쓰자
enum class AffectedReason : uint8_t {
	Unit,
	Alliance,
	Item,
	Permernent,		// 영구효과 스킬
	Buff,
	Debuff,
};

enum class AffectedSituation : uint8_t {
	Active,
	Always,
	OnAttack,
	OnUpdate,
	OnOneSec,
	OnHit,
	State,
	Count,
};

class SkillLevelData
{
public:
	vector<pair<SkillEffect, float>> effects;
};

typedef uint16_t SKILL_ID;
class SkillData
{
public:
	SKILL_ID ID;
	wstring name;
	uint8_t maxLevel;
	SkillShape shape;
	SkillTarget target;
	SkillDispel dispel;
	AffectedSituation situation;
	vector<SkillLevelData> levelData;

	static uint16_t Num;
	static bool InitSkillTemplate(wstring path);
	static const SkillData* GetSkillTemplate(SKILL_ID ID);
	static void UninitSkillTemplate();
	static std::unordered_map<SkillEffect, std::function<bool(float, class FLBattleData*, class FLBattleData*)>> OnHit;
};

struct AffectedSkill {
public:
	const static float INFINITE;

	AffectedSkill(SKILL_ID skillID_, uint8_t level_, AffectedReason reason_, int reasonID_, float endTime_ = INFINITE)
		:skillID(skillID_), level(level_), reason(reason_), reasonID(reasonID_), endTime(endTime_) {};
	SKILL_ID skillID = 0;
	uint8_t level = 0;
	AffectedReason reason = AffectedReason::Unit;
	int reasonID = 0;	// cast 해서 넣을것, 혹은 Union 을 쓸것
	float endTime = 0;
};
