#include "SkillData.h"
#include "CSVParser.h"
#include "EnochFreeLancerData.h"
#include "EnochSimulator.h"

// 컴파일 전에 uint16 테스트하기

#include <map>
#include <memory>

const float AffectedSkill::INFINITE = 99999;
std::unordered_map<SkillEffect, std::function<bool(float, FLBattleData*, FLBattleData*)>> SkillData::OnHit;

#define	MACRO_MAP_ELEM(STRUCT, STR)	{L#STR, STRUCT::STR}

#define MACROEFFECT(STR) MACRO_MAP_ELEM(SkillEffect, STR)
static const map<wstring, SkillEffect> EffectInit =
{
	MACROEFFECT(HP),
	MACROEFFECT(HPGEN),
	MACROEFFECT(MP),
	MACROEFFECT(MPGEN),
	MACROEFFECT(ATKDMG),
	MACROEFFECT(ATKSPD),
	MACROEFFECT(ATKRNG),
	MACROEFFECT(DEF),
	MACROEFFECT(MAGREG),
	MACROEFFECT(MAGDMG),
	MACROEFFECT(PUREDMG),
	MACROEFFECT(MULTIEFFECT),
	MACROEFFECT(SKILLRANGE),
	MACROEFFECT(TARGETNUM),
	MACROEFFECT(SKILLWIDTH),
	MACROEFFECT(STUN),
	MACROEFFECT(EXPLOSIONRANGE),
	MACROEFFECT(CRITICAL),
	MACROEFFECT(PERCENT),
};

#define MACROSHAPE(STR) MACRO_MAP_ELEM(SkillShape, STR)
static const map<wstring, SkillShape> ShapeInit =
{
	MACROSHAPE(Active),
	MACROSHAPE(Passive),
	MACROSHAPE(Projectile),
};

#define MACROTARGET(STR) MACRO_MAP_ELEM(SkillTarget, STR)
static const map<wstring, SkillTarget> TargetInit =
{
	MACROTARGET(Ally_All),
	MACROTARGET(Enemy_All),
	MACROTARGET(Enemy_Single),
	MACROTARGET(Enemy_Linear),
	MACROTARGET(Enemy_Around),
};

#define MACRODISPEL(STR) MACRO_MAP_ELEM(SkillDispel, STR)
static const map<wstring, SkillDispel> DispelInit =
{
	MACRODISPEL(None),
};

#define MACROSITUATION(STR) MACRO_MAP_ELEM(AffectedSituation, STR)
static const map<wstring, AffectedSituation> SituationInit =
{
	MACROSITUATION(Active),
	MACROSITUATION(Always),
	MACROSITUATION(OnAttack),
	MACROSITUATION(OnUpdate),
	MACROSITUATION(OnOneSec),
	MACROSITUATION(OnHit),
	MACROSITUATION(State),
};

static map<uint16_t, shared_ptr<SkillData>> mSkillTemplate;
uint16_t SkillData::Num = 0;

bool SkillData::InitSkillTemplate(wstring path)
{
	const static wstring filename = L"SkillData.csv";
	wstring path_data = path + L"/" + filename;

	CSVParser parser(path_data);
	if (!parser.success)
		return false;

	const auto& content = parser.content;
	const unsigned size_data = content.size();

	if (!mSkillTemplate.empty())
		SkillData::UninitSkillTemplate();

	// 첫줄은 헤더라 무시, idx 를 1부터 시작
	unsigned idx = 1;
	while (idx < size_data)
	{
		shared_ptr<SkillData> elem(new SkillData());

		unsigned tail = idx;
		while (++tail < size_data && content[tail][0].empty())
			;
		const uint8_t level = tail - idx;
		elem->maxLevel = level;
		elem->ID = static_cast<SKILL_ID>(stoi(content[idx][0]));
		elem->name = content[idx][1];

#define MACROFIND(MAP, CONTENT, DEFAULT) MAP.find(CONTENT) != MAP.end() ? MAP.find(CONTENT)->second : DEFAULT

		elem->shape = MACROFIND(ShapeInit, content[idx][2], SkillShape::Passive);
		elem->situation = MACROFIND(SituationInit, content[idx][3], AffectedSituation::Active); //사용방법 컬럼은 일단 보류
		elem->target = MACROFIND(TargetInit, content[idx][4], SkillTarget::Ally_All);
		elem->dispel = MACROFIND(DispelInit, content[idx][5], SkillDispel::None);

		for (uint8_t i = 0; i < level; i++)
		{
			const SkillEffect effect = MACROFIND(EffectInit, content[idx + i][6], SkillEffect::HP);
			const float value = stof(content[idx + i][7]);
			SkillLevelData data;
			if (effect == SkillEffect::MULTIEFFECT)
				for (int iEffect = 0; iEffect < value; iEffect++) {
					const SkillEffect _effect = MACROFIND(EffectInit, content[idx + i][8 + iEffect * 2], SkillEffect::HP);
					const float _value = stof(content[idx + i][9 + iEffect * 2]);
					data.effects.push_back(make_pair(_effect, _value));
				}
			else
				data.effects.push_back(make_pair(effect, value));
			elem->levelData.push_back(data);
		}

		mSkillTemplate.insert(make_pair(elem->ID, elem));
		idx += level;
	}

	OnHit = decltype(OnHit)();
	OnHit[SkillEffect::STUN] = [](float time, FLBattleData* attacker, FLBattleData* defender) -> bool
	{
		if (defender->state == FreeLancerState::Dead) return false;
		AffectedSkill stun(-1, -1, AffectedReason::Debuff, (int)SkillEffect::STUN, time);
		defender->affectedList[(int)AffectedSituation::State].push_back(stun);
		defender->SetState(FreeLancerState::Idle);
		EnochSimulator::logs.push({ SimulateInfoType::Buff, defender->GetSerialNumber(), (int)SkillEffect::STUN });
		return true;
	};
	OnHit[SkillEffect::CRITICAL] = [](float multiplier, FLBattleData* attacker, FLBattleData* defender) -> bool
	{
		if (defender->state == FreeLancerState::Dead) return false;
		auto damage = FLBattleData::CalculateDamage(attacker->attackDamage * multiplier, defender);

		defender->mpNow = std::min((double)defender->mpMax, defender->mpNow + std::min(damage / 9.5, 75.0));
		attacker->mpNow = std::min((double)attacker->mpMax, attacker->mpNow + std::min(damage / 7.5, 10.0));
		EnochSimulator::logs.push({ SimulateInfoType::CriticalDamage, attacker->GetSerialNumber(), defender->GetSerialNumber(), (int)damage});
		return false;
	};


	SkillData::Num = mSkillTemplate.size();
	return true;
}

void SkillData::UninitSkillTemplate()
{
	mSkillTemplate.clear();
	OnHit.clear();
	return;
}

const SkillData* SkillData::GetSkillTemplate(SKILL_ID ID)
{
	auto it = mSkillTemplate.find(ID);
	if (it != mSkillTemplate.end())
		return it->second.get();
	return nullptr;
}