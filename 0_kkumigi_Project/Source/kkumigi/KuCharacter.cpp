// Fill out your copyright notice in the Description page of Project Settings.


#include "KuCharacter.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "FieldItemFactory.h"
#include "ItemManager.h"
#include "KuGameInstance.h"
#include "KuInputConfig.h"
#include "KuPlayData.h"
#include "KuPlayerController.h"
#include "KuUIManager.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Voxel/Chunk/ChunkManager.h"
#include "Voxel/Utils/VoxelFunctionLibrary.h"
#include "Voxel/World/ChunkWorld.h"

#include "UTLStatics.h"

AKuCharacter::AKuCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//카메라가 회전(컨트롤러가 회전)할 때 캐릭터가 같이 회전하는 거 방지. 
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw   = false;
	bUseControllerRotationRoll  = false;

	// 무브먼트 구성
	GetCharacterMovement()->bUseControllerDesiredRotation = false;                     // 컨트롤러 방향으로 회전. 조준 중일 땐 같이 회전함.
	GetCharacterMovement()->bOrientRotationToMovement     = true;                      // 가속 방향으로 회전.
	GetCharacterMovement()->RotationRate                  = FRotator(0.0, 700.0, 0.0); // 기본값. 회전속도. 초당 회전 변화량. 속도 변경가능.
	GetCharacterMovement()->SetWalkableFloorAngle(45.5f);

	// 카메라 암 구성
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);   // 캐릭터가 회전할 때 카메라가 따라 회전하는 거 방지
	CameraBoom->bDoCollisionTest         = false; // 충돌할 때 카메라 암이 줄어드는거 방지

	// 카메라 암 랙
	CameraBoom->bEnableCameraLag         = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraLagSpeed           = 10.f;
	CameraBoom->CameraRotationLagSpeed   = 10.f;

	// 카메라 암 각도 및 위치
	CameraBoom->TargetArmLength = 2700.0f;                         //2100.0f;
	CameraBoom->SocketOffset    = FVector(0.0);                    // 카메라 상대 위치
	CameraBoom->TargetOffset    = FVector(0.0, 0.0, 30.0);         // 원점 상대 위치
	CameraBoom->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f)); //(FRotator(-30.0f, 0.0f, 0.0f));


	// 카메라 구성
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	PlayerCamera->SetProjectionMode(ECameraProjectionMode::Perspective);
	PlayerCamera->bUsePawnControlRotation = false; // 카메라 암이 회전할 때 회전 방지

	PlayerCamera->SetFieldOfView(32.f);
	PlayerCamera->SetRelativeRotation(FRotator(0.0));

	// 카메라 포스트 프로세싱
	PlayerCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance   = true;
	PlayerCamera->PostProcessSettings.bOverride_DepthOfFieldDepthBlurAmount = true;
	PlayerCamera->PostProcessSettings.bOverride_DepthOfFieldDepthBlurRadius = true;

	PlayerCamera->PostProcessSettings.DepthOfFieldFocalDistance   = CameraBoom->TargetArmLength; // 500.0f;
	PlayerCamera->PostProcessSettings.DepthOfFieldDepthBlurAmount = 0.5f;
	PlayerCamera->PostProcessSettings.DepthOfFieldDepthBlurRadius = 6.0f;


	InteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionCapsule"));
	InteractionCapsule->SetupAttachment(RootComponent);

	VoxelDetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("VoxelDetectionBox"));
	VoxelDetectionBox->SetupAttachment(RootComponent);
}

void AKuCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetWorld())
	{
		for (APostProcessVolume* PPVolume : TActorRange<APostProcessVolume>(World))
		{
			if (PPVolume && PPVolume->ActorHasTag(TEXT("MainPostProcessVolume")))
			{
				MainPostProcessVolume = PPVolume;
				break;
			}
		}
	}
}

void AKuCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AChunkWorld* ChunkWorld = GetChunkWorld())
	{
		ChunkWorld->UpdatePlayerLocation(GetActorLocation());
	}
}

void AKuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	const UKuInputConfig* PCInputConfig        = Cast<AKuPlayerController>(GetController())->InputActionsConfig;
	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComp->BindAction(PCInputConfig->IA_Move, ETriggerEvent::Triggered, this, &AKuCharacter::MoveCameraInput);
	EnhancedInputComp->BindAction(PCInputConfig->IA_LeftButton, ETriggerEvent::Triggered, this, &AKuCharacter::LeftButtonInput);
	EnhancedInputComp->BindAction(PCInputConfig->IA_RightButton, ETriggerEvent::Triggered, this, &AKuCharacter::RightButtonInput);
}

void AKuCharacter::MoveCameraInput(const FInputActionValue& InputValue)
{
	if (IsValid(Controller) == false)
		return;

	const FVector2D InputVector = InputValue.Get<FVector2D>();

	// 전방 찾기
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, InputVector.X);
	AddMovementInput(RightDirection, InputVector.Y);
}

void AKuCharacter::LeftButtonInput(const FInputActionValue& InputValue)
{
	if ((GetKuGameInstance()->GetUIManager()->IsInventoryOpen() == false)
		&& (GetKuGameInstance()->GetItemManager()->GetFieldItemFactory()->IsItemPreviewMode() == false))
	{
		AddVoxels();
	}
}

void AKuCharacter::RightButtonInput(const FInputActionValue& InputValue)
{
	if ((GetKuGameInstance()->GetUIManager()->IsInventoryOpen() == false)
		&& (GetKuGameInstance()->GetItemManager()->GetFieldItemFactory()->IsItemPreviewMode() == false))
	{
		RemoveVoxels();
	}
}

TArray<FVector> AKuCharacter::GetVoxelDetectionBoxBounds()
{
	const FVector CenterPos  = VoxelDetectionBox->GetComponentLocation();
	const FVector Extent     = VoxelDetectionBox->GetScaledBoxExtent();
	const double VoxelLength = GetGameInstance<UKuGameInstance>()->GetChunkManager()->GetUnitVoxelLength();

	FVector RoundPos = UtlMath::RoundVector(CenterPos, VoxelLength * 2.0f);
	RoundPos.Z = CenterPos.Z;

	const TArray<FVector> VoxelPositions = UVoxelFunctionLibrary::BoxVoxelWorldPositionsByExtent(RoundPos, Extent, VoxelLength);

	return VoxelPositions;
}

void AKuCharacter::AddVoxels()
{
	GetChunkWorld()->ModifyChunks(GetVoxelDetectionBoxBounds(), EBlockType::Stone);
}

void AKuCharacter::RemoveVoxels()
{
	GetChunkWorld()->ModifyChunks(GetVoxelDetectionBoxBounds(), EBlockType::Air);
}

UKuGameInstance* AKuCharacter::GetKuGameInstance() const
{
	return GetGameInstance<UKuGameInstance>();
}

AChunkWorld* AKuCharacter::GetChunkWorld() const
{
	return GetKuGameInstance()->GetChunkManager()->GetChunkWorld();
}
