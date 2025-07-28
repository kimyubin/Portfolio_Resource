// Fill out your copyright notice in the Description page of Project Settings.


#include "ShellMaterialMeshComponent.h"


//~=======================
// SceneProxy
class FMyCustomSceneProxy : public FPrimitiveSceneProxy
{
public:
	// 생성자
	FMyCustomSceneProxy(UShellMaterialMeshComponent* Component): FPrimitiveSceneProxy(Component)
	, OffsetDirection()
	, Amount(Component->ShellCount)
	, MaterialInterface(Component->GetMaterial(0))
	{
		// 필요한 데이터 초기화
	}

	// Scene Proxy에서 메시 렌더링을 위한 메서드
	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		// Amount 만큼 반복하며 여러 겹을 생성
		for (int32 i = 0; i < Amount; i++)
		{
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];

					// 기본 메시 요소 가져오기
					FMeshBatch& Mesh = Collector.AllocateMesh();

					// 기본 메시 데이터 설정
					Mesh.bWireframe = false;  // 와이어프레임 모드
					Mesh.MaterialRenderProxy = MaterialInterface->GetMaterial()->GetRenderProxy(); // 0번 머티리얼 사용

					// 현재 셸의 idx에 따른 오프셋 값 적용
					FMatrix OffsetTransform = FMatrix::Identity;
					OffsetTransform.SetOrigin(OffsetDirection * i); // i 값에 따라 오프셋 적용

					// 메시의 트랜스폼 설정
					Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
					Mesh.Type = PT_TriangleList;
					Mesh.DepthPriorityGroup = SDPG_World;
					Mesh.LCI = NULL;
					Mesh.LODIndex = 0;
					Mesh.bCanApplyViewModeOverrides = false;

					// 메시 트랜스폼 추가
					// Mesh.LocalToWorld = GetLocalToWorld() * OffsetTransform;

					// 메시 컬렉터에 추가
					Collector.AddMesh(ViewIndex, Mesh);
				}
			}
		}
	}

	// 바운딩 박스 계산
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;
	virtual uint32 GetMemoryFootprint(void) const override;

protected:
	// 필요한 데이터 저장
	FVector OffsetDirection;
	int32 Amount;  // 최대 겹수
	UMaterialInterface* MaterialInterface;
	
};
//~=======================


UShellMaterialMeshComponent::UShellMaterialMeshComponent(): ShellMaterial(nullptr), ShellStaticMesh(nullptr)
{
	CreateShells();
}

FPrimitiveSceneProxy* UShellMaterialMeshComponent::CreateSceneProxy()
{
	return Super::CreateSceneProxy();
}

void UShellMaterialMeshComponent::CreateShells()
{
	if (ShellStaticMesh == nullptr || ShellMaterial == nullptr)
		return;

	for (int32 idx = 0; idx < ShellCount; ++idx)
	{
		if (ShellStaticMesh->GetMaterial(idx) == nullptr)
			ShellStaticMesh->AddMaterial(ShellMaterial);
	}
	if (ShellStaticMesh->GetMaterial(ShellCount))
	{
		for (int32 idx = ShellCount; idx < 1'000; ++idx)
		{
			if (ShellStaticMesh->GetMaterial(idx) == nullptr)
				break;

			ShellStaticMesh->SetMaterial(idx, nullptr);
		}
		UStaticMesh::RemoveUnusedMaterialSlots(ShellStaticMesh);
	}
}

void UShellMaterialMeshComponent::UpdateShellParameters()
{/*
	for (int32 idx = 0; idx < ShellCount; ++idx)
	{
		UMaterialInterface* ShellMatInterface = ShellStaticMesh ? ShellStaticMesh->GetMaterial(idx) : nullptr;
		UMaterial* ShellMat                   = ShellMatInterface ? ShellMatInterface->GetMaterial() : nullptr;
		if (ShellMat == nullptr)
			continue;
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_ShellIndex"), idx);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_ShellCount"), ShellCount);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_ShellLength"), ShellLength);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_Density"), Density);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_Thickness"), Thickness);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_Attenuation"), Attenuation);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_Curvature"), Curvature);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_ShellDistanceAttenuation"), ShellDistanceAttenuation);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_DisplacementStrength"), DisplacementStrength);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_OcclusionBias"), OcclusionBias);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_NoiseMin"), NoiseMin);
		ShellMat->SetScalarParameterValueEditorOnly(TEXT("_NoiseMax"), NoiseMax);

		ShellMat->SetVectorParameterValueEditorOnly(TEXT("_ShellColor"), ShellColor);
	}*/
}

