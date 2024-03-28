#pragma once

#include "ProjectileAct.h"
class PJGuidedArrow : public ProjectileAct
{
public:
	void _Begin();
	void _Tick(float deltaTime);
	void _End();
};
