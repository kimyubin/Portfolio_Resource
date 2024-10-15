// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayDefaultPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
FRotator APlayDefaultPawn::CameraArmRotate = FRotator::ZeroRotator;
// Sets default values
APlayDefaultPawn::APlayDefaultPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	DefaultPawnCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DefaultPawnCamera"));

	//카메라 기본 셋팅. 상속받은 블루프린트에서 수정가능.
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeLocationAndRotation(FVector(10, 230, 20), FRotator(-60, 0, 0));
	CameraBoom->TargetArmLength = 3000.f;
	CameraBoom->bDoCollisionTest = false;				// 레벨과 충돌할 때 카메라 암이 줄어드는거 방지(당겨지는거 방지)
	CameraBoom->bEnableCameraLag = false;				// 카메라 이동 랙 없음. 초기 위치 변경시 랙 발생.
	CameraBoom->bEnableCameraRotationLag = true;		// 카메라 각도 조정만 랙 있음. 자연스러운 움직임용.
	
	DefaultPawnCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);	
	DefaultPawnCamera->SetRelativeLocationAndRotation(FVector::ZeroVector,FRotator::ZeroRotator);
	DefaultPawnCamera->SetFieldOfView(32.f);			// 카메라 화각 조정.
	DefaultPawnCamera->bUsePawnControlRotation = false; // 카메라 암이 회전할 때 같이 회전하는거 방지

	//각도 조정용 기준 값. 블루프린트에서 수정 가능.
	TotalNumberOfSteps = 20;
	MaxAngle = -80.f;

	//기준 기본값을 기반으로 생성된 값들.	
	//MaxAngle을 TotalNumberOfSteps 단계로 쪼갬. 기본값은 중간인 60도 = 15단계	
	TargetAngleStep = 15;
	UnitAngle = MaxAngle / TotalNumberOfSteps;
}

// Called when the game starts or when spawned
void APlayDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	//카메라용 기본 pawn 위치 고정. 없으면 멋대로 위치가 튀는 경우가 있음
	SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);

	CameraArmRotate = CameraBoom->GetRelativeRotation();
	//블루프린트에서 수정 후 초기화용
	TargetAngleStep = 15;
	UnitAngle = MaxAngle / TotalNumberOfSteps;
}

// Called every frame
void APlayDefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//용병 기울기용
	FRotator SpringArmAngle = CameraBoom->GetRelativeRotation();
	const float TargetAngle = UnitAngle * TargetAngleStep;
	if (TargetAngle != SpringArmAngle.Pitch)	
		CameraArmRotate = SpringArmAngle;
	
}

// Called to bind functionality to input
void APlayDefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MouseWheel"), this, &APlayDefaultPawn::ZoomInOut);
}

void APlayDefaultPawn::ZoomInOut(float Axis)
{
	//카메라 각도 조정
	if ((Controller != nullptr) && (Axis != 0.0f))
	{		
		if(Axis>0)
			TargetAngleStep++;
		else if(Axis<0)
			TargetAngleStep--;
		TargetAngleStep = FMath::Clamp(TargetAngleStep, 1, TotalNumberOfSteps);

		//입력된 MaxAngle과 현재 TargetAngleStep을 바탕으로 목표 각도 계산
		FRotator SpringArmAngle = CameraBoom->GetRelativeRotation();		
		const float TargetAngle = UnitAngle * TargetAngleStep;
		if (TargetAngle != SpringArmAngle.Pitch)
		{
			SpringArmAngle.Pitch = TargetAngle;
			CameraBoom->SetRelativeRotation(SpringArmAngle);
		}
	}

}