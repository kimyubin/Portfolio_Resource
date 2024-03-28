// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelForeverTypes.h"
#include "GameFramework/Actor.h"
#include "CitadelBuildingFactory.generated.h"

/**
 * 성채, 빌딩 등은 본 클래스를 통해 생성, 해제, 관리됨.
 * 월드가 바뀌면 자동으로 리셋
 * 월드별 리셋되어야하는 데이터만 존재해야함
 */
UCLASS()
class CITADELFOREVER_API ACitadelBuildingFactory : public AActor
{
	GENERATED_BODY()
	
public:	
	ACitadelBuildingFactory();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	/** 임시 빌딩 스폰 및 커서 위치 추적*/
	class ACitadelBuilding* SpawnPreviewBuilding(FBuildingInfo InBuildingInfo);
	
	/** 미리보기 건물을 실제로 변경 후 건설모드로 전환.*/
	bool BuildPreviewToReal();
	
	/** 스폰 건물 리스트에 추가. 각 건물이 beginPlay할때 알아서 추가함.*/
	void AddBuilding(const TWeakObjectPtr<class ACitadelBuilding> InBuilding);
	
	/** 파괴된 건물 리스트에서 제거. 파괴되는 건물이 endplay에서 작동시킴 */
	void RemoveBuilding(const TWeakObjectPtr<class ACitadelBuilding> InBuilding);
	

protected:
	/** 설치된 성벽 및 타워 객체 리스트 */
	TSet<TWeakObjectPtr<class ACitadelBuilding>> InstalledBuildingList;
	
	/** 미리보기용 임시 빌딩 액터*/
	TWeakObjectPtr<class ACitadelBuilding> BF_Weak_PreviewBuilding;
	
	/** 현재 선택된 빌딩*/
	EBuildingType SelectedBuilding;

public:
	EBuildingType GetSelectedBuilding() const;
	
	/** 빌딩 선택 및 */
	void SelectBuilding(EBuildingType InBuildingType);
};
