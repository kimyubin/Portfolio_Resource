#pragma once

#include "ProjectileAct.h"
#include "../EnochGeometry.h"
#include <set>

//for SkillTarget::Enemy_Around
class PJExplosion : public ProjectileAct
{
public:
	virtual void _Begin() override;
	virtual void _Tick(float deltaTime) override;
	virtual void _End() override;
	float width = 0.0f;
private :
	void DoAttack();
};
