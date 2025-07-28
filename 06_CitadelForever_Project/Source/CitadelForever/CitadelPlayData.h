// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CitadelForeverTypes.h"
#include "UObject/NoExportTypes.h"
#include "CitadelPlayData.generated.h"

/**
 * 상속된 BP 사용
 */
UCLASS(Blueprintable, BlueprintType)
class CITADELFOREVER_API UCitadelPlayData : public UObject
{
	GENERATED_BODY()
	
public:	
	UCitadelPlayData();

	// Function
	
	/**
	 * 골드 변경
	 * @param ResChanges 소모/획득 변화량 
	 */
	bool ChangeGold(int32 ResChanges);

	/** 빌딩 타입별 기본 클래스 반환. BP로 작성된 클래스를 반환함.*/
	TSubclassOf<class ACitadelBuilding> GetBuildingClass(uint8 BuildingTypeNum);
	/** 빌딩 타입별 기본 클래스 반환. BP로 작성된 클래스를 반환함.*/
	TSubclassOf<class ACitadelBuilding> GetBuildingClass(EBuildingType BuildingType);
	/** UI 슬롯용 이미지 반환*/
	UTexture2D* GetBuildingUIImage(uint8 BuildingTypeNum);
	/** UI 슬롯용 이미지 반환*/
	UTexture2D* GetBuildingUIImage(EBuildingType BuildingType);
protected:

	// Variable
	
	/** 현재 자원량 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Resources")	
	uint32 GoldsAvailable;

	/** 누적 자원량 */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Resources")	
	uint32 GoldsGathered;

	/** 각 빌딩 클래스 및 UI 이미지 모음. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Buildings")	
	TArray<FBuildingTemplate> BuildingClassList;
	
	/** 타워 스텟 정보. */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Buildings")	
	UDataTable* TowerStatusData;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Buildings")	
	UMaterial* BuildableMaterial;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Buildings")	
	UMaterial* UnbuildableMaterial;



public:
		
	/** 사용가능 골드*/
	uint32 GetGoldsAvailable() const { return GoldsAvailable; }
	/** 누적 골드*/
	uint32 GetGoldsGathered() const { return GoldsGathered; }

	UMaterial* GetBuildableMaterial() const { return BuildableMaterial; }
	UMaterial* GetUnbuildableMaterial() const { return UnbuildableMaterial; }

};
