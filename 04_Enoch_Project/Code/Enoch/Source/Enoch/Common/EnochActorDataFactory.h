// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include<unordered_map>
#include<queue>
#include<memory>
#include<set>

using namespace std;

/**
 *  액터 데이터 관리자
 *  미리생성하는 로직은 밀어버리고, 제한된 인덱스를 재활용하는 방식 사용
 */
class EnochActorDataFactory
{
public:
	EnochActorDataFactory();
	~EnochActorDataFactory();

	void BeginPlay();
	void EndPlay();

	inline int SpawnFreeLancer(); //프리랜서 데이터를 하나 스폰하고, 인덱스(시리얼넘버) 리턴
	inline int SpawnProjectile(); //투사체 데이터를 하나 스폰하고, 인덱스(시리얼넘버) 리턴
	
	inline void DeleteProjectile(int SerialNumber); //투사체 인덱스를 반납하고 데이터 제거
	inline void DeleteFreeLancer(int SerialNumber); //용병 인덱스를 반납하고 데이터 제거
	
	inline class FLBattleData* GetFreeLancerData(int SN) {
		return _FreeLancerMap.find(SN) == _FreeLancerMap.end() ? nullptr : _FreeLancerMap.at(SN).get();
	}

	inline class EnochProjectileData* GetProjectileData(int SN) {
		return _ProjectileMap.find(SN) == _ProjectileMap.end() ? nullptr : _ProjectileMap.at(SN).get();
	}
	static EnochActorDataFactory *instance;

private :

	unordered_map<int, shared_ptr<class FLBattleData>> _FreeLancerMap;

	//투사체가 할당된 맵
	unordered_map<int, shared_ptr<class EnochProjectileData>> _ProjectileMap;
	int _SerialNumber_FreeLancer;
	int _SerialNumber_Projectile;
	int actorID;

	/** 동맹 관련 */
public:
	// AllianceID, <Level, Member> 순서
	typedef unordered_map<uint8_t, pair<uint8_t, shared_ptr<set<uint8_t>>>> AllianceStruct;
private:
	AllianceStruct allianceAlly;
	AllianceStruct allianceEnemy;

public:
	void UpdateAllianceAll();
	void UpdateFLAll();
	const AllianceStruct& GetAlliance (bool isEnemy) const
	{
		if (isEnemy)	return allianceEnemy;
		else			return allianceAlly;
	}
	// 얼라이언스 레벨은 1부터 시작...
	uint8_t GetAllianceLevel(bool isEnemy, uint8_t allianceID);
	wstring strAlly, strEnemy;
};
