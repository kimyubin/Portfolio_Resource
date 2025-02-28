// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include "KuGameInstance.generated.h"

class UChunkManager;
class AChunkWorld;
class UKuUIManager;
class UItemManager;
class UKuPlayData;
class UKuSaveManager;

/**
 * 
 */
UCLASS()
class KKUMIGI_API UKuGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UKuGameInstance();
	virtual void Init() override;
	virtual void Shutdown() override;

	void PrevLevelBeginPlay();
	void PostLevelBeginPlay();

	UKuPlayData* GetKuPlayData() const;
	UKuSaveManager* GetKuSaveManager() const;
	UItemManager* GetItemManager() const;
	UKuUIManager* GetUIManager() const;
	UChunkManager* GetChunkManager() const;

	
	void GetOnlineSubSystem();


	/** PlayData BP. BP에서 등록한 데이터를 가져오는데 사용합니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<UKuPlayData> KuPlayDataClass;

	/** UIManger BP */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<UKuUIManager> KuUIManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Defined Class")
	TSubclassOf<UChunkManager> ChunkManagerClass;

private:
	/**
	 * BP에서 정의된 데이터를 cpp에서 용이하게 사용하기 위한 데이터셋입니다.
	 * BP를 이곳에 등록해 사용합니다.
	 */
	UPROPERTY()
	TObjectPtr<UKuPlayData> PlayData;

	/** 세이브 로직 매니저 */
	UPROPERTY()
	TObjectPtr<UKuSaveManager> SaveManager;

	/** 아이템 관련 로직 매니저 */
	UPROPERTY()
	TObjectPtr<UItemManager> ItemManager;

	UPROPERTY()
	TObjectPtr<UKuUIManager> UIManager;

	UPROPERTY()
	TObjectPtr<UChunkManager> ChunkManager;
};
