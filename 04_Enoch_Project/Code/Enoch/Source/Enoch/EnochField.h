// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "Common/EnochFieldData.h"
#include "EnochField.generated.h"

/*
* 레벨에 하나가 생성된다고 가정하고 설계된 클래스
* static함수를 통해 필드셀을 인덱스로 리턴하기 위해 존재
 */

UCLASS()
class ENOCH_API AEnochField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnochField();	
	static FVector GetLocation(float x, float y);
private:
	static AEnochField* Cell;
public:
	static class AEnochFieldCell *GetCell(FVector2D FieldLocation) {
		if (Cell == nullptr) return nullptr;
		else if (FieldLocation.Y >= Cell->Field->Num() || FieldLocation.X >= (*Cell->Field)[FieldLocation.Y].Num())
			return nullptr;
		return (*Cell->Field)[FieldLocation.Y][FieldLocation.X];
	}

	UFUNCTION(BlueprintCallable, Category = "Field")
		static class AEnochFieldCell* GetCell(FIntPoint FieldLocation) {
		if (Cell == nullptr) return nullptr;
		else if (FieldLocation.Y >= Cell->Field->Num() || FieldLocation.X >= (*Cell->Field)[FieldLocation.Y].Num())
			return nullptr;
		return (*Cell->Field)[FieldLocation.Y][FieldLocation.X];
	}
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	static void forAllAttachedFreeLancer(std::function<void(class AEnochFreeLancer *, FIntPoint)> func);
	/**
	 * 저장된 필드 용병 전체를 필드에 전체 생성/스폰.(적,아군 포함 전체)
	 */
	UFUNCTION(BlueprintCallable, Category="Field")
    void GenerateField();
private :
	//EnochFieldData::GetWidth()로 대체
	// int32 height = 10;
	// int32 width = 10;
	TUniquePtr<TArray<TArray<class AEnochFieldCell *>>> Field;

	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* Mesh;

	EnochFieldData data;
};