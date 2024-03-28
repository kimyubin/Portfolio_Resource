// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "GameFramework/Actor.h"
#include "Common/EnochFreeLancerdata.h"
#include "FLAnimLoader.generated.h"

UCLASS()
class ENOCH_API AFLAnimLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFLAnimLoader();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	static AFLAnimLoader* instance;
	static USkeletalMesh *GetMesh(uint8 templateID);
	static TSubclassOf<UAnimInstance> *GetAnim(uint8 templateID);

private :
	//메쉬 정보들
	static TMap<uint8, USkeletalMesh*> Meshes;
	static TMap<uint8, TSubclassOf<UAnimInstance>> Anims;
	static bool LoadModelFiles();	//리턴값 의미없음
	static void LoadMeshFile(const wchar_t* path, uint8 type);
	static void LoadAnimFile(const wchar_t* path, uint8 type);

};
