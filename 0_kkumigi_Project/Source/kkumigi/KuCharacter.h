// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KuCharacter.generated.h"

class UKuGameInstance;
class UBoxComponent;
class AChunkWorld;
struct FInputActionValue;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class KKUMIGI_API AKuCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AKuCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveCameraInput(const FInputActionValue& InputValue);

	void LeftButtonInput(const FInputActionValue& InputValue);
	void RightButtonInput(const FInputActionValue& InputValue);

protected:
	TArray<FVector> GetVoxelDetectionBoxBounds();
	void AddVoxels();
	void RemoveVoxels();

	UKuGameInstance* GetKuGameInstance() const;
	AChunkWorld* GetChunkWorld() const;

	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<APostProcessVolume> MainPostProcessVolume;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraComponent, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CameraComponent, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> InteractionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> VoxelDetectionBox;
	
};
