#pragma once
#include "EnochFreeLancerAct.h"
#include "../SkillData.h"

class FLRangeSkill :
    public FreeLancerAct
{
public:
	virtual void _Begin() override;
	virtual void _End() override;
	SkillData skillData;
	int target;
};

