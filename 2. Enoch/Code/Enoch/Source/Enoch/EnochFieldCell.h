// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Enoch.h"
#include "EnochField.h"
#include "Common/EnochFieldCellData.h"
#include "Materials/Material.h"
#include "GameFramework/Actor.h"
#include "EnochFieldCell.generated.h"

/*
 * 인게임에서 필드의 한칸을 담당하는 액터. 역할은 세가지
 * 1. 마우스입력이 어디에 전달되었는지 감지
 * 2. 실제 데이터에게 입력 처리 요청
 * 3. 현재 자신을 밟고있는 프리랜서 기억
 */

UCLASS()
class ENOCH_API AEnochFieldCell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnochFieldCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UFUNCTION(BlueprintCallable, Category = "FieldCell")
	class AEnochFreeLancer* GetAttachedFreeLancer();

	UFUNCTION(BlueprintCallable, Category = "FieldCell")
	void AttachFreeLancer(class AEnochFreeLancer* arg);

	UFUNCTION(BlueprintCallable, Category = "FieldCell")
	void DetachFreeLancer();

	UFUNCTION(BlueprintCallable, Category = "FieldCell")
	FVector2D GetFieldLocation() {
		if (data == nullptr) return FVector2D::ZeroVector;
		else return FVector2D((int32)(data->GetLocation().x), (int32)(data->GetLocation().y));
	}
	UFUNCTION(BlueprintCallable, Category = "FieldCell")
	bool HasAttachFreeLancer();

//private :
	void SetFieldLocation(int32 inputY, int32 inputX);
	static UMaterial *WhiteMaterial;
	static UMaterial *BlackMaterial;
	static bool InitMaterial();
	EnochFieldCellData *data;
	int AttachedFreeLancer = -1;
	friend class AEnochField;
};

