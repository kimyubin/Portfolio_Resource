#include "FLMove.h"
#include "../EnochFieldData.h"
#include "../EnochFieldCellData.h"
#include "../EnochFreeLancerData.h"
#include "../EnochActDelay.h"
#include "../EnochSimulator.h"

void FLMove::_Begin()
{
	auto currentFieldCell = EnochFieldData::GetData(prevLocation);
	auto targetFieldCell = EnochFieldData::GetData(postLocation);
	//direction = postLocation - prevLocation;	//don't used yet
	length = 0.5f;
	time = 0;
	floatLocation = VectorF2D(prevLocation);
	currentFieldCell->SetFreeLancerOnFight(-1);
	//targetFieldCell->SetFreeLancerOnFight(data);
	data->locationOnFight = targetFieldCell->GetLocation();
	data->direction = postLocation - prevLocation;
	EnochSimulator::logs.push({ SimulateInfoType::Move, data->GetSerialNumber() });
}

//reserve target Field Cell
void FLMove::_End()
{
	direction = prevLocation + (postLocation - prevLocation);
}

void FLMove::_Tick(float deltaTime)
{
	static double PI_Q = PI / 2;
	time += deltaTime;
	auto moveDelay = std::min(0.3f, length);
	auto moveTime = time;
	if (moveTime > moveDelay) moveTime = moveDelay;
	
	auto location2D = (postLocation - prevLocation) *  sin(PI_Q * (moveTime / moveDelay));
	location2D = location2D + prevLocation;
	floatLocation = location2D;
}