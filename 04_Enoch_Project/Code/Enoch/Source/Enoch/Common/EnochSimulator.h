// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include <unordered_map>
#include "EnochFreeLancerData.h"
#include "EnochProjectileData.h"

using namespace std;

/*
* 언리얼엔진의 틱 방식 그대로 사용
 */

UENUM()
enum class GameResult : uint8 {
	None,
	UnderSimulated,
	Win,
	Lose,
	Draw,
};

UENUM()
enum class SimulateInfoType : uint8 {
	Move,
	Attack,
	ChangeState,
	Damage,
	CriticalDamage,
	Skill,
	SpawnPJ,
	RemovePJ,
	Debug,
	Buff,
	EndSimulator,
	Dead,
};

struct SimulateInfo {
	SimulateInfoType type;
	int val1;
	int val2;
	int val3;
};

class EnochSimulator
{
public:
	EnochSimulator();
	~EnochSimulator();
	void BeginPlay();
	void EndPlay();
	void BeginTick(float DeltaTime);
	shared_ptr<pair<vector<int>, vector<int>>> Tick(float DeltaTime);

	//시뮬레이터 결과
	GameResult result;
	static queue<SimulateInfo> logs;
	static EnochSimulator* instance;
	
private:
	//큐에 사용될 노드
	template<typename T>
	struct Node {
		T* data;
		float time;
	};

	//노드의 정렬에 사용될 비교함수. 가장 최근의 일을 먼저 리턴하도록 정렬
	template<typename T>
	struct comp {
		bool operator()(struct Node<T> &lhs, struct Node<T> &rhs) {
			return lhs.time > rhs.time;
		}
	};

	template<typename T>
	using PriorityQueue = priority_queue < struct Node<T>, vector<Node<T>>, comp<T> >;

	//template<typename T>
	//using QueuePtr = unique_ptr <PriorityQueue<T>>;

public:
	static float simulateTime;	//시뮬레이터 내 시간
private:
	float simulateEndTime;	//시뮬레이터가 종료될 시간
	const float postDelay = 3.0f;	//시뮬레이터가 종료된 후 판정이 일어날 때 까지의 시간
	int numPlayerFreeLancer;	//아군수
	int numEnemyFreeLancer;		//적군수
	bool simulating;	//시뮬레이터가 작동중인 경우 true
	


	void FreeLancerTick(FLBattleData *, float);
	void ProjectileTick(EnochProjectileData *, float);

public:
	//용병 우선순위 큐
	unique_ptr<vector<int>> FreeLancerQueue;

	//투사체 우선순위 큐
	unique_ptr<vector<int>> ProjectileQueue;

};