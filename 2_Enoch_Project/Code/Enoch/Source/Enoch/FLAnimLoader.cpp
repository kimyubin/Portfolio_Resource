// Fill out your copyright notice in the Description page of Project Settings.


#include "FLAnimLoader.h"

TMap<uint8, USkeletalMesh*> AFLAnimLoader::Meshes;
TMap<uint8, TSubclassOf<UAnimInstance>> AFLAnimLoader::Anims;
AFLAnimLoader* AFLAnimLoader::instance = nullptr;

// Sets default values
AFLAnimLoader::AFLAnimLoader()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LoadModelFiles();
	instance = this;
}

// Called when the game starts or when spawned
void AFLAnimLoader::BeginPlay()
{
	Super::BeginPlay();
}

void AFLAnimLoader::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto pair : Meshes)
	{
		if(pair.Value != nullptr) delete pair.Value;
	}
}

bool AFLAnimLoader::LoadModelFiles()
{
/*
	define문 수정할일 있을시 참고
	EXAMPLE(Type) 호출시
	##Type을 사용하면 그냥 Type이랑 동일한 동작이지만, 다른문자나 토큰과 띄어쓰기없이 붙어있어도 동작하기때문에 함수호출등에 활용 가능. 단, 맥에서 컴파일 안되는것 확인되어 사용 안함
	#를 사용하면 문자열로 변함. "Type"이라는 문자열로 바꾸고싶을 때 사용
*/
#define MODEL_FOLDER() "/Game/DataForCli/Animations/"
#define TYPEFULLNAME(type) FreeLancerTemplateID::type
#define MESH_PATH(Type) TEXT(MODEL_FOLDER()#Type"/Mesh.Mesh")
#define ANIM_PATH(Type) TEXT(MODEL_FOLDER()#Type"/AnimController.AnimController_C")
#define REGISTER_MODEL(Type) \
    LoadMeshFile(TCHAR_TO_WCHAR(MESH_PATH(Type)), TYPEFULLNAME(Type)); \
    LoadAnimFile(TCHAR_TO_WCHAR(ANIM_PATH(Type)), TYPEFULLNAME(Type));

    //REGISTER_MODEL(Melee1);
    //REGISTER_MODEL(Melee2);

	return true;
}

void AFLAnimLoader::LoadMeshFile(const wchar_t* path, uint8 type)
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshFile(WCHAR_TO_TCHAR(path));
	if (MeshFile.Succeeded())
		Meshes.Add(type, MeshFile.Object);
}
void AFLAnimLoader::LoadAnimFile(const wchar_t* path, uint8 type)
{
	ConstructorHelpers::FClassFinder<UAnimInstance> AnimFile(WCHAR_TO_TCHAR(path));
	if (AnimFile.Succeeded())
		Anims.Add(type, AnimFile.Class);
}

USkeletalMesh* AFLAnimLoader::GetMesh(uint8 templateID)
{
	//if (instance == nullptr) return nullptr;
	auto mesh = instance->Meshes.Find(templateID);
	if (mesh == nullptr) return nullptr;
	else return *mesh;
}

TSubclassOf<UAnimInstance> *AFLAnimLoader::GetAnim(uint8 templateID)
{
	//if (instance == nullptr) return nullptr;
	auto anim = instance->Anims.Find(templateID);
	if (anim == nullptr) return nullptr;
	return anim;
}
