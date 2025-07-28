// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "ShellMaterialMeshComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class KKUMIGI_API UShellMaterialMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:
	UShellMaterialMeshComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	int ShellCount = 16;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float ShellLength = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float ShellDistanceAttenuation = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float Density = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float NoiseMin = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float NoiseMax = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float Thickness = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float Attenuation = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float Curvature = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float DisplacementStrength = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	FLinearColor ShellColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	float OcclusionBias = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	UMaterialInterface* ShellMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	UStaticMesh* ShellStaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shell Settings")
	TArray<UStaticMeshComponent*> Shells;

public:
	UFUNCTION(CallInEditor)
	void CreateShells();
	UFUNCTION(CallInEditor)
	void UpdateShellParameters();
};

