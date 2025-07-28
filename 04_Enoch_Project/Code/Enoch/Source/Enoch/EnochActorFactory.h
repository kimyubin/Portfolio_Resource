// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "Common/EnochFreeLancerData.h"
#include "Common/EnochActorDataFactory.h"
#include "GameFramework/Actor.h"
#include "EnochActorFactory.generated.h"

/*
* 레벨에 한개가 존재한다고 가정하고 작성
* 맵에 액터 수를 조절하기 위한 풀링 클래스
* Common 소스코드에 의사결정을 맡기고, 그 결과를 UE4에 반영만 함
*/
UCLASS()
class ENOCH_API AEnochActorFactory : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnochActorFactory();
	
	//용병 액터 생성
	UFUNCTION(BlueprintCallable, Category = "EnochActorFactory")
	static class AEnochFreeLancer* SpawnFreeLancerByTID(uint8 type, uint8 level, bool isEnemy=false);
	static class AEnochFreeLancer* SpawnFreeLancerBySN(int32 SerialNumber);

	//투사체 액터를 생성
	static class AEnochProjectile* SpawnProjectile(int32 SerialNumber);	
	UFUNCTION(BlueprintCallable, Category = "EnochActorFactory")
	static void Release(class AEnochFreeLancer* freeLancer, bool update);	//다 쓴 프리랜서를 반납
	static void Release(class AEnochProjectile* freeLancer);	//다 쓴 투사체를 반납

	UFUNCTION(BlueprintCallable, Category = "EnochActorFactory")
	static class AEnochFreeLancer* GetFreeLancer(int32 SerialNumber);	//해당 시리얼넘버를 갖는 프리랜서를 바로 얻음. 
	static class AEnochProjectile* GetProjectile(int32 SerialNumber);  //해당 시리얼넘버를 갖는 투사체를 바로 얻음.

	static class FLBattleData* GetFreeLancerData(int32 SerialNumber);
	static class EnochProjectileData* GetProjectileData(int32 SerialNumber);
	inline static int GetFreeLancerMax() { return Factory == nullptr ? -1 : Factory->FreeLancerMap.Num(); };
	//static class AEnochActor* GetActor(int32 SerialNumber);	//해당 시리얼넘버를 갖는 액터 리턴. EnochActor로 업캐스팅되어있으므로 알아서 다운캐스팅해야함
	
	/** 저장된 필드 용병의 자동 스폰 절차에서 Factory 초기화여부 확인용. 초기화 이후에 스폰하게끔. EnochField에서 사용.
	 * @return Factory 초기화(begin Play)작업이 완료되면 true;  */
	static bool GetFactoryReady();

	static void ClearProjectiles();
private :
	static AEnochActorFactory *Factory;
	TUniquePtr<EnochActorDataFactory> CommonFactory;
	static bool FactoryReady;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private :
	
	//오브젝트 저장용 맵
	TMap<int, class AEnochFreeLancer *> FreeLancerMap;
	TMap<int, class AEnochProjectile *> ProjectileMap;

	//TID, class
	TMap<int, TSubclassOf<class AEnochFreeLancer>> FreelancerTemplateClass;
	void loadFreelancerTemplateClass(int TID, const TCHAR *path);

	//TID, class
	TMap<int, TSubclassOf<class AEnochProjectile>> ProjectileTemplateClass;
	void loadProjectileTemplateClass(int TID, const TCHAR* path);
	
	//UPROPERTY(EditDefaultsOnly)
	//int32 ActorNumberLimit = 100;
public:
	static void UpdateAlliance();
};