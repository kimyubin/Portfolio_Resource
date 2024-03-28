#include "Commander.h"

#include <random>
#include <algorithm>


#include "Field/FieldSystemNoiseAlgo.h"
#include "Math/UnitConversion.h"

uint16 EnochCommander::getExpForNextLevel( uint8 level )
{
	const uint8 nextLevel = level + 1;
	switch(nextLevel)
	{
		case 1 :	return 0;	break;
		case 2 :	return 1;	break;
		case 3 :	return 1;	break;
		case 4 :	return 5;	break;
		case 5 :	return 10;	break;
		case 6 :	return 15;	break;
		case 7 :	return 20;	break;
		case 8 :	return 25;	break;
		case 9 :	return 30;	break;
		case 10 :	return 35;	break;
		case 11 :	return 40;	break;
		case 12 :	return 45;	break;
		case 13 :	return 50;	break;
		case 14 :	return 55;	break;
		case 15 :	return 60;	break;
		case 16 :	return 65;	break;
		case 17 :	return 70;	break;
		case 18 :	return 75;	break;
		case 19 :	return 80;	break;
		case 20 :	return 85;	break;
		default : break;
	}
	return 1;
}

uint16 EnochCommander::getMaxFreeLancerNumber( uint8 level )
{
	if( level <= 0 || level > LEVEL_MAX)
		return 0;
	switch(level)
	{
	case 1 :	return 3;	break;
	case 2 :	return 3;	break;
	case 3 :	return 3;	break;
	default :	return level; break;
	}
	return 1;
}

uint32 EnochCommander::getRecruitProb( uint8 level, std::vector<uint32> &probList )
{
	if(probList.size() < 5)
		probList.resize(5);
	switch(level)
	{
	case 1:
		probList[0] = 80, probList[1] = 20; break;
	case 2:
		probList[0] = 70, probList[1] = 30; break;
	case 3:
		probList[0] = 55, probList[1] = 35, probList[2] = 10; break;
	case 4:
		probList[0] = 45, probList[1] = 40, probList[2] = 15; break;
	case 5:
		probList[0] = 35, probList[1] = 40, probList[2] = 25; break;
	case 6:
		probList[0] = 25, probList[1] = 35, probList[2] = 35, probList[3] = 5; break;
	case 7:
		probList[0] = 20, probList[1] = 30, probList[2] = 40, probList[3] = 10; break;
	case 8:
		probList[0] = 18, probList[1] = 24, probList[2] = 35, probList[3] = 20, probList[4] = 3; break;
	case 9:
		probList[0] = 15, probList[1] = 21, probList[2] = 30, probList[3] = 28, probList[4] = 6; break;
	case 10:
		probList[0] = 12, probList[1] = 18, probList[2] = 28, probList[3] = 32, probList[4] = 10; break;
	default:	break;
	}
	uint32 total = 0;
	for(auto elem : probList)
		total += elem;
	return total;
}


EnochCommander::EnochCommander()
{
	abilityPoint = 0;
	gold = 0;
	level = 1;
	exp = 0;

	floor = 1;
	life = DEFAULT_LIFE;
	countWin = 0;
	countLose = 0;

	CurrentProgress = EGameProgress::None;
	fieldFreeLancerCount = 0;
	
	RecruitFreeLancerList.assign(MAX_RECRUITSLOT,FLSaveData());
	InvenFreeLancerList.assign(MAX_INVENSLOT,FLSaveData());
	CombinePossibleList.assign(MAX_RECRUITSLOT,FreeLancerCombineData());
		
	FieldFreeLancerList = vector<vector<FLSaveData>>(EnochFieldData::GetHeight(),vector<FLSaveData>(EnochFieldData::GetWidth(),FLSaveData()));

	// 적풀 3개 * 등급 5개 * 각 용병( TID, count)
	EnemyPoolFLbyGradeList =
		vector<vector<vector<pair<uint8, int>>>>(
			3,vector<vector<pair<uint8, int>>>(
				5,vector<pair<uint8, int>>()));

	EnemyPoolSelectInfo = EnemyPoolSelectionState();
}

uint32 EnochCommander::addExp(uint32 add)
{
	if(level >= LEVEL_MAX)
		return exp;
	exp += add;
	do
	{
		const uint32 expNeed = EnochCommander::getExpForNextLevel( level );
		if( exp < expNeed )
			break;
		level++;
		if(level >= LEVEL_MAX)
			break;
		exp -= expNeed;
    } while(1);
	return exp;
}
std::pair<FailReason, int> EnochCommander::BuyFreeLancer(const int& RecruitListIdx)
{
	auto TargetFLTID = RecruitFreeLancerList[RecruitListIdx].FLTemplateID;
	// 상점 슬롯에 용병이 없음
	if(TargetFLTID == FreeLancerTemplateID::None)
		return make_pair(FailReason::None, 0);
	//일단 구매 비용은 용병 등급으로 맞춤. 
	if (gold < FreeLancerTemplate::GetFreeLancerTemplate(TargetFLTID)->buyPrice)
		return make_pair(FailReason::NotEnoughGolds, 0);

	//구매 가능여부 중간결과
	return make_pair(FailReason::Success, FreeLancerTemplate::GetFreeLancerTemplate(TargetFLTID)->buyPrice);
}

FailReason EnochCommander::BuyFLToInven(const int &RecruitListIdx, const int &InvenListIdx)
{
	auto BuyFLResult = BuyFreeLancer(RecruitListIdx);
	if (BuyFLResult.first != FailReason::Success)
		return BuyFLResult.first;
	
	//인벤 슬롯에 용병이 이미 있음
	if(InvenFreeLancerList[InvenListIdx].FLTemplateID!=FreeLancerTemplateID::None)
		return FailReason::None;

	//구매 및 자리 이동 성공.
	gold -= BuyFLResult.second;
	
	InvenFreeLancerList[InvenListIdx] = RecruitFreeLancerList[RecruitListIdx];
	RecruitFreeLancerList[RecruitListIdx] = FLSaveData();
	
	return FailReason::Success;
}
FailReason EnochCommander::BuyDisposeFLToField(const int& RecruitIdx, const Vector2D& FieldCell)
{
	auto BuyFLResult = BuyFreeLancer(RecruitIdx);
	if (BuyFLResult.first != FailReason::Success)
		return BuyFLResult.first;
	
	//용병 최대치 도달해서 구매 실패
	if(!IsValidFieldCount())
		return FailReason::FieldAlreadyFull;	
	//필드에 용병 있음
	if(FieldFreeLancerList[FieldCell.y][FieldCell.x].FLTemplateID!=FreeLancerTemplateID::None)
		return FailReason::None;
	
	//구매 및 자리 이동 성공.
	gold -= BuyFLResult.second;
	
	FieldFreeLancerList[FieldCell.y][FieldCell.x] = RecruitFreeLancerList[RecruitIdx];
	RecruitFreeLancerList[RecruitIdx] = FLSaveData();

	fieldFreeLancerCount++;
	return FailReason::Success;
}
void EnochCommander::BringFLFieldToInven(const Vector2D& FieldCell, const int& InvenIdx)
{
	//인벤에 자리가 이미있으면 대기
	if(InvenFreeLancerList[InvenIdx].FLTemplateID != FreeLancerTemplateID::None)
		return;
	InvenFreeLancerList[InvenIdx] = FieldFreeLancerList[FieldCell.y][FieldCell.x];
	FieldFreeLancerList[FieldCell.y][FieldCell.x] = FLSaveData();
	fieldFreeLancerCount--;	
}
FailReason EnochCommander::DisposeFLInvenToField(const Vector2D& FieldCell, const int& InvenIdx)
{
	//용병 최대치 도달해서 스폰 실패
	if(!IsValidFieldCount())
		return FailReason::FieldAlreadyFull;
	//인벤에 용병 없으면 대기
	if(InvenFreeLancerList[InvenIdx].FLTemplateID == FreeLancerTemplateID::None)
		return FailReason::None;
	FieldFreeLancerList[FieldCell.y][FieldCell.x] = InvenFreeLancerList[InvenIdx];
	InvenFreeLancerList[InvenIdx] = FLSaveData();
	fieldFreeLancerCount++;
	
	return FailReason::Success;
}
const uint8_t EnochCommander::FLSellPriceCalc(const FLSaveData& FL)
{
	// 올림. price*(합성용병수)^(level-1)*(1-(level-1)*0.125).
	// 1레벨 구매원가 * 합성에 사용되는 총 용병의 수 * 용병 레벨 별 패널티 계산	
	uint8_t price = FreeLancerTemplate::GetFreeLancerTemplate(FL.FLTemplateID)->buyPrice;
	uint8_t sellPrice = static_cast<uint8_t>(ceil(
		price * pow(COMBINE_NUM + 1, (FL.FLLevel - 1)) * (1 - (FL.FLLevel - 1) * 0.125)));
	return sellPrice;
}

void EnochCommander::SellInvenFreelancer(const int& InvenIdx)
{
	FLSaveData FL = InvenFreeLancerList[InvenIdx];
	if (FL.FLTemplateID == FreeLancerTemplateID::None)
		return;

	gold += FLSellPriceCalc(FL);

	InvenFreeLancerList[InvenIdx] = FLSaveData();
	//합성 재탐색
	CombineRecruitList();
}

void EnochCommander::MoveFLInsideField(const Vector2D& AVec2D, const Vector2D& BVec2D)
{
	auto TempA = FieldFreeLancerList[AVec2D.y][AVec2D.x];	
	FieldFreeLancerList[AVec2D.y][AVec2D.x] = FieldFreeLancerList[BVec2D.y][BVec2D.x];
	FieldFreeLancerList[BVec2D.y][BVec2D.x] = TempA;
}

void EnochCommander::SwapInvenSlot(const int &Aidx, const int &Bidx)
{
	auto TempA = InvenFreeLancerList[Aidx];
	InvenFreeLancerList[Aidx] = InvenFreeLancerList[Bidx];
	InvenFreeLancerList[Bidx] = TempA;
}
FreeLancerCombineData EnochCommander::FindFLCanCombine(FLSaveData FindFL)
{
	//1성 구매후 2성 합성, 2성 합성 완료 후 3성완료 연쇄 작용까지 고려

	FreeLancerCombineData result = FreeLancerCombineData();
	result.FinalCombineFL = FindFL;
	
	int countInvenFL = 0;
	int countFiledFL = 0;
	
	if(FindFL.FLTemplateID == FreeLancerTemplateID::None) return result;
	
	//각 리스트에서 중복된 용병 탐색
	for(int i = 0; i < MAX_INVENSLOT;++i)
	{
		if (InvenFreeLancerList[i] == FindFL)
		{
			countInvenFL++;
			result.InvenList.push_back(i);
		}
	}
	//필드는 아군만 탐색
	for(int i = 0; i<EnochFieldData::GetHeight()/2;++i)
	{
		for(int j = 0; j<EnochFieldData::GetWidth();++j)
		{
			if(FieldFreeLancerList[i][j] == FindFL)
			{
				countFiledFL++;
				result.FieldList.push_back({i,j});
			}
		}
	}
	//1개(Combine_num) 이상 있으면 레벨업하고, 다시 탐색 후 추가
	//if(countInvenFL>=1|| countFiledFL>=1)
	if((countInvenFL + countFiledFL) >= COMBINE_NUM)
	{
		result.CanCombine = true;
		result.FinalCombineFL.FLLevel++;
		auto tempResult =  FindFLCanCombine(result.FinalCombineFL);
		//재귀 호출 마지막까지 들어가면 추가 안하고 바로 리턴		
		if(!tempResult.CanCombine)
			return result;
		
		result.InvenList.insert(result.InvenList.end(), tempResult.InvenList.begin(), tempResult.InvenList.end());
		result.FieldList.insert(result.FieldList.end(), tempResult.FieldList.begin(), tempResult.FieldList.end());
		//렙업한 용병 집어넣음
		result.FinalCombineFL = tempResult.FinalCombineFL;		
	}
	//없으면 찾아낸 용병 추가 안하고 리턴
	return result;	
}
void EnochCommander::CombineRecruitList()
{
	for(int i = 0; i < MAX_RECRUITSLOT;++i)
	{
		//구매 후 빈 슬롯 검색 안하기
		if(RecruitFreeLancerList[i].FLTemplateID==FreeLancerTemplateID::None)
			continue;

		//합성 여부 판단은 1레벨 부터 시작함. 구매 직후 2레벨 이상인걸 1레벨로 초기화(2명 합성일때 문제 발생)
		RecruitFreeLancerList[i].FLLevel = 1;
		
		//인벤, 필드 훑어봐서 합성 가능한게 있는지 탐색 후
		CombinePossibleList[i] = FindFLCanCombine(RecruitFreeLancerList[i]);
		//렙업하면 렙업한 걸로 업데이트
		if(CombinePossibleList[i].CanCombine)
			RecruitFreeLancerList[i] = CombinePossibleList[i].FinalCombineFL;
		//렙업 대상이 아니면 1레벨로 초기화.
		//초기화가 없으면
		//다른 슬롯에서 합성된 용병을 구매해도, 현 슬롯이 1레벨 이상으로 남아있게됨.
		//어차피 없으면 무조건 1레벨임
		else
			RecruitFreeLancerList[i].FLLevel = 1;
	}
}

bool EnochCommander::CombineDelFL(const FreeLancerCombineData& CombineIdx)
{
	if(!CombineIdx.CanCombine) return false;
	if(!CombineIdx.FieldList.empty())
	{		
		for(auto it = CombineIdx.FieldList.begin();it!=CombineIdx.FieldList.end();++it)
		{
			FieldFreeLancerList[it->y][it->x] = FLSaveData();
			--fieldFreeLancerCount;
		}
	}
	if(!CombineIdx.InvenList.empty())
	{
		for(auto it = CombineIdx.InvenList.begin();it!=CombineIdx.InvenList.end();++it)
		{
			InvenFreeLancerList[*it] = FLSaveData();
		}
	}
	return true;
}


bool EnochCommander::IsValidFieldCount()
{
	//현재 규칙.
	//1, 2레벨은 3명. 그 이후 레벨만큼 배치 가능
	//실패시 경고창
	if(level < 3)
	{
		if(fieldFreeLancerCount == 3)
			return false;		
	}
	else
	{
		if(fieldFreeLancerCount == level)
			return false;		
	}
	return true;
		
}
void EnochCommander::FriendToHalfList()
{
	int Height = EnochFieldData::GetHeight();
	HalfFieldList.clear();
	HalfFieldList.reserve(Height / 2);
	//아군필드 절반 가져옴
	HalfFieldList.assign(FieldFreeLancerList.begin(), FieldFreeLancerList.begin() + (Height / 2));
	
}

void EnochCommander::HalfToEnemyList()
{
	int Height = EnochFieldData::GetHeight();
	
	//절반 필드 역순
	reverse(HalfFieldList.begin(), HalfFieldList.end());
	FieldFreeLancerList.reserve(Height);
	//적군 필드에 붙임.
	for(int i = 0; i < Height/2; ++i)
	{
		//copy시 배열 범위 위험때문에 컴파일 에러
		FieldFreeLancerList[i+Height/2] = HalfFieldList[i];	
	}	
}
void EnochCommander::HalfToFriendList()
{
	int Height = EnochFieldData::GetHeight();
	
	FieldFreeLancerList.reserve(Height);
	//적군 필드에 붙임.
	for(int i = 0; i < Height/2; ++i)
	{
		//copy시 배열 범위 위험때문에 컴파일 에러
		FieldFreeLancerList[i] = HalfFieldList[i];	
	}	
}

