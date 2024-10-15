// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MontisPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MontisInputConfig.h"
#include "InputMappingContext.h"
#include "Montis.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AMontisPlayerController::AMontisPlayerController()
{
	
}

void AMontisPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	SetInputModeGameOnly();
	
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
}

void AMontisPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AMontisPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	const auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	//입력 다시 맵핑
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);
	
	const auto EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	
}
void AMontisPlayerController::SetInputModeGameOnly()
{
	//마우스 입력 모드 게임 전용. 컨트롤러만 반응
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
}
void AMontisPlayerController::SetInputModeGameAndUI()
{
	//마우스 입력 모드 게임 + UI 모두 입력 가능.
	//클릭시 커서 사라지는 거 방지
	FInputModeGameAndUI InputModeGameUI = FInputModeGameAndUI();
	InputModeGameUI.SetHideCursorDuringCapture(false);
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(true);
}


void AMontisPlayerController::GetMultiHitResultsUnderCursor(TArray<FHitResult>& OutHitResults, ECollisionChannel TraceChannel)
{
	FVector2D ScreenPosition;
	FVector WorldLocation;
	FVector WorldDirection;

	Cast<ULocalPlayer>(Player)->ViewportClient->GetMousePosition(ScreenPosition);

	//뷰포트에서 찍은 곳의 월드 위치 구해서 EndPoint에 넣기
	UGameplayStatics::DeprojectScreenToWorld(this, ScreenPosition, WorldLocation, WorldDirection);

	const FVector StartPoint = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->K2_GetActorLocation();
	const FVector EndPoint = (WorldDirection * 15000) + WorldLocation;

	//카메라에서 마우스 찍은 곳까지 라인 트레이스.
	//채널은 BuildingLineTrace. 기본적으로 오버랩으로 처리되어있음.
	GetWorld()->LineTraceMultiByChannel(OutHitResults, StartPoint, EndPoint, TraceChannel);
}

void AMontisPlayerController::RotateCameraBoomToTarget(const FVector& ActorLocation, const FVector& ChangeCameraOffset, ECollisionChannel TraceChannel)
{
	const auto CurrentCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector CurrentCamLocation = CurrentCamera->K2_GetActorLocation();
	const FVector LineTraceEndPoint = (CurrentCamera->GetActorForwardVector() * 15000) + CurrentCamLocation;
	FVector TargetLocation = LineTraceEndPoint;		// 히트 대상 없으면 적당히 먼거리로 갈음함.
	bool bIsRotatableDist = true;	//회전 가능한 거리인지 판단. 너무 가깝거나, 현재 y축 회전량이 수직(89~271(-89))에 근접하면 회전 불가 판정.
	
	FHitResult HitResult; // 조준 대상 위치 추적
	GetWorld()->LineTraceSingleByChannel(HitResult, CurrentCamLocation, LineTraceEndPoint, TraceChannel);

	if (HitResult.bBlockingHit)
	{
		TargetLocation = HitResult.Location;

		/**
		 * 너무 가까운 거리는 의도와 다르게 너무 많이 회전함.
		 * 수직에 가깝게 위아래를 바라보면, x축까지 같이 회전하게 됨. 상하 89도 이상 넘어가면 회전불가 판정.
		 */
		double ControlPitch = GetControlRotation().Pitch;
		ControlPitch = ControlPitch < 0 ? 360.0 + ControlPitch : ControlPitch;

		if ((HitResult.Location - ActorLocation).Size() < 130.0 || (89.0 < ControlPitch && ControlPitch < 271.0))
			bIsRotatableDist = false;
	}
		
	
	//아직 목표 각도를 한번에 계산하지 못하고, 루프를 통해 목표까지 회전 찾음.	
	//목표지점에 근사하게 회전하거나, 지정된 횟수 이상 계산 후 루프 탈출.
	int count = 0;
	while (count < 10 && bIsRotatableDist)
	{
		// 변경될 오프셋에 현재 위치와 회전값을 적용했을때, 예상되는 카메라의 상대 위치와 절대 위치 계산
		FVector RotatedRelativeLocation;
		FVector RotatedWorldLocation;
		FMatrix CurrentRotationMatrix = FRotationMatrix(GetControlRotation());// 현재 회전량의 회전행렬

		// 변경할 오프셋을 현재 회전량으로 회전시켰을 때 상대 위치 계산.
		RotatedRelativeLocation = CurrentRotationMatrix.TransformPosition(ChangeCameraOffset);
		RotatedWorldLocation = ActorLocation + RotatedRelativeLocation;
		
		//현재 조준 중인 타겟을 다음 카메라 위치에서도 바라보기 위한 각도 계산.
		//카메라 스프링암이 아닌, 카메라 자체가 회전했을 때의 각도라서 약간 틀어짐
		FRotator ChangedRotator = UKismetMathLibrary::FindLookAtRotation(RotatedWorldLocation, TargetLocation);
		 
		FRotator PrevGetControlRotation = GetControlRotation();
		//스프링암 회전은 컨트롤러 회전을 따라가기 때문에, 컨트롤러 회전을 변경
		SetControlRotation(ChangedRotator);

		// 교체할 값이 아주 작은 값으로 근사한 경우, 각도 변경이 되지 않음. 변경되지 않을 경우 탈출		
		if(FMath::IsNearlyEqual(PrevGetControlRotation.Yaw, GetControlRotation().Yaw))
			break;
		count++;
	}
	
	DrawDebugLine(GetWorld(), CurrentCamLocation, TargetLocation, FColor(255, 0, 0), false, 5, 0, 0.7);
}


void AMontisPlayerController::TestRotateControllerTowardTargetForCamera(FTransform ActorTransform, FVector CurrentCamOffset,  FVector TargetCameraOffset, ECollisionChannel TraceChannel)
{
	const auto CurrentCamera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector StartLocation = CurrentCamera->K2_GetActorLocation();
	const FVector EndPoint = (CurrentCamera->GetActorForwardVector() * 15000) + StartLocation;
	FVector TargetLocation = EndPoint;		// 히트 대상 없으면 적당히 먼거리로 갈음함.
	
	FHitResult HitResult;	// 조준 대상 위치 추적
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndPoint, TraceChannel);

	if (HitResult.bBlockingHit)
	{
		TargetLocation = HitResult.Location;
	}
	
	// 피치	y-축을 기준으로 x-z 평면의 각
	// 요	z-축을 기준으로 x-y 평면의 각
	// 롤	x-축을 기준으로 y-z 평면의 각

	//피치 요 롤
	FVector ActorLocation = ActorTransform.GetLocation();
	FVector TargetRelativeLocation = TargetLocation - ActorLocation;
	FVector CurrentCamLocation = StartLocation;
	FVector CurrentCamRelativeLocation = StartLocation - ActorLocation;
	FVector ChangeCamLocation ;
	FVector ChangeCamRelativeLocation ;
	FMatrix RotationMatrix = FRotationMatrix(GetControlRotation()); // 현재 회전량의 회전행렬
	ChangeCamRelativeLocation = RotationMatrix.TransformPosition(TargetCameraOffset);
	ChangeCamLocation = ActorTransform.GetLocation() + ChangeCamRelativeLocation;
	
	
	auto distCal = [](double a, double b) { return sqrt(a * a + b * b); };
	std::vector<double> print(9);
	double xzBase = print[1] = distCal(TargetRelativeLocation.X, TargetRelativeLocation.Z);
	double xyBase = print[2] = distCal(TargetRelativeLocation.X, TargetRelativeLocation.Y);
	double xzCurHeight = print[3] = CurrentCamRelativeLocation.Z;
	double xzChangHeight = print[4] = ChangeCamRelativeLocation.Z;
	double xyCurHeight = print[5] = CurrentCamRelativeLocation.Y;
	double xyChangHeight = print[6] = ChangeCamRelativeLocation.Y;

	double xzAng = print[7] = cos(xzCurHeight / xzBase) - cos(xzChangHeight / xzBase);
	double xyAng = print[8] = cos(xyCurHeight / xyBase) - cos(xyChangHeight / xyBase);
	
	FRotator nnrotator( GetControlRotation().Pitch+ FMath::RadiansToDegrees(xzAng), GetControlRotation().Yaw+  FMath::RadiansToDegrees(xyAng), GetControlRotation().Roll);

	SetControlRotation(nnrotator);
	for (int i = 0; i < print.size();++i)
	{
		MT_LOG("%d: %f", i, print[i])
	}
	MT_LOG("TargetLocation %s", *nnrotator.ToString())
	MT_LOG("------------------------")


	DrawDebugLine(
		GetWorld(),
		StartLocation,
		TargetLocation,
		FColor(255, 0, 0),
		false, 5, 0,
		0.7
	);
}
