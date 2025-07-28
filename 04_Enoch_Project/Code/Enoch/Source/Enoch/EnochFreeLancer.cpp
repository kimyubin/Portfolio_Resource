// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFreeLancer.h"
#include "Common/EnochFreeLancerdata.h"
#include "EnochField.h"
#include "EnochFieldCell.h"
#include "EnochPaperFlipbookComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "FLAnimInstance.h"
#include "FLAnimLoader.h"
#include "Components/WidgetComponent.h"
#include "Math/Quat.h"
#include "UIs/FreeLancerDeathUI.h"
#include "Common/FLActs/FLMove.h"
#include "Components/CapsuleComponent.h"
#include "UIs/FieldDragDetector.h"
#include "UIs/FreeLancerUnifiedUpperUI.h"


// Sets default values
AEnochFreeLancer::AEnochFreeLancer()
{	
	//LineTrace 감지용 태그
	Tags.Add(FName(TEXT("EnochFreeLancer")));
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CAPSULE"));
	CapsuleComponent->AttachToComponent(AnimGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CapsuleComponent->SetRelativeLocation({0.f,0.f,55.f});
    CapsuleComponent->SetRelativeScale3D({KINDA_SMALL_NUMBER, 1.25f, 1.f});
	CapsuleComponent->SetGenerateOverlapEvents(false);
	CapsuleComponent->SetCollisionProfileName(TEXT("LineTraceOnly"));
	

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MESH"));
	MeshComponent->AttachToComponent(AnimGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->SetRelativeScale3D({ 0.5,0.5,0.5 });

	//UI컴포넌트 생성 및 부착
	UnifiedUpperUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("UnifiedUI"));
	DeathWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DeathText"));
	DragDetectWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DragDetector"));

	//UI컴포넌트에 HP바 설정
	static ConstructorHelpers::FClassFinder<UUserWidget> UnifiedUpperUI(TEXT("/Game/UI/FreeLancerUnifiedUpperUIBP"));
	static ConstructorHelpers::FClassFinder<UUserWidget> DeathUI(TEXT("/Game/UI/FreeLancerDeathText"));
	static ConstructorHelpers::FClassFinder<UUserWidget> DragDetectUI(TEXT("/Game/UI/FieldDragDetector"));

	
	if (UnifiedUpperUI.Succeeded())
	{
		UnifiedUpperUIComp->SetWidgetClass(UnifiedUpperUI.Class);
		UnifiedUpperUIComp->SetDrawSize(FVector2D(100,50));
		UnifiedUpperUIComp->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);
		UnifiedUpperUIComp->SetRelativeLocation(FVector(0.,0.,125.f));
		UnifiedUpperUIComp->SetWidgetSpace(EWidgetSpace::Screen);
	}			
	if(DeathUI.Succeeded())
	{
		DeathWidgetComp->SetWidgetClass(DeathUI.Class);
		DeathWidgetComp->SetDrawSize(FVector2D(65,25));
		DeathWidgetComp->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);
		DeathWidgetComp->SetRelativeLocation(FVector(0.,0.,180.f));
		DeathWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	}
	//주의. 이 컴포넌트는 반드시 마지막에 Attach되어야 함. 다른 UI에 가려져선 안됨.
	if(DragDetectUI.Succeeded())
	{
		DragDetectWidgetComp->SetWidgetClass(DragDetectUI.Class);
		DragDetectWidgetComp->SetDrawSize(FVector2D(150,180));
		DragDetectWidgetComp->AttachToComponent(AnimGroupComponent,FAttachmentTransformRules::KeepRelativeTransform);
		DragDetectWidgetComp->SetRelativeLocation(FVector(0.,0.,20.f));
		DragDetectWidgetComp->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		DragDetectWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	}
}

void AEnochFreeLancer::InitFL(int SN)
{
	// 이 시점에서 이미 데이터가 초기화 되었으므로 다시 초기화(InitStatus 등) 안한다...
	auto data = GetData();
	auto TID = data->GetTID();
	if (TID == FreeLancerTemplateID::None) return;
	if (data == nullptr) return;

	// 이름 및 HP바 설정
	//사망 애니메이션 접근
	//드래그 감지용 위젯 설정
	UFreeLancerUnifiedUpperUI* UnifiedUpperUI = Cast<UFreeLancerUnifiedUpperUI>(UnifiedUpperUIComp->GetUserWidgetObject());
	UFreeLancerDeathUI* DeathAnim =Cast<UFreeLancerDeathUI>(DeathWidgetComp->GetUserWidgetObject());
	UFieldDragDetector* DragDetectWidget = Cast<UFieldDragDetector>(DragDetectWidgetComp->GetWidget());

	if(UnifiedUpperUI!=nullptr)
		UnifiedUpperUI->SetOwnerFreeLancer(this);	
	if(DeathAnim!=nullptr)
		DeathAnim->SetOwnerFreeLancer(this);
	if(DragDetectWidget!=nullptr)
		DragDetectWidget->SetOwnerFreeLancer(this);
	SetDragDetectOnOff(false);

}
FVector2D AEnochFreeLancer::GetFieldLocation() 
{ 
	auto data = GetData();
	if (data == nullptr) return FVector2D(0, 0);
	else return FVector2D(data->location.x, data->location.y); 
}

FreeLancerState AEnochFreeLancer::GetState() 
{ 
	auto data = GetData();
	return data != nullptr ? data->GetState() : (FreeLancerState)0; 
}
//return valid data or nullptr
FLBattleData *AEnochFreeLancer::GetData()
{
	return AEnochActorFactory::GetFreeLancerData(SerialNumber);
}

void AEnochFreeLancer::Release(bool update)
{
	AEnochActorFactory::Release(this, update);
}

void AEnochFreeLancer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//Common 클래스는 가비지컬렉션이 작동하지않으므로, 스스로 메모리를 정리할 수 있도록 유도
	auto data = GetData();
	if (data == nullptr) return;
	data->EndPlay();
}

void AEnochFreeLancer::Reset()
{
	auto data = GetData();
	if (data == nullptr) return;
	//data->SetState(FreeLancerState::Idle);
	//data->SetTarget(nullptr);
	//initial rotation
	Flipbook->SetRelativeRotation(UKismetMathLibrary::MakeRotFromX(FVector(0, 1, 0)));
	UnifiedUpperUIComp->SetVisibility(true);
	Flipbook->Idle();

	data->hpNow = data->hpMax;
	data->mpNow = 0;
}

void AEnochFreeLancer::Move()
{
	auto x = GetData()->direction.y;
	if (x != 0)
		Flipbook->SetRelativeRotationExact(UKismetMathLibrary::MakeRotFromX(FVector(0, x > 0 ? 90 : -90, 0)));

	Flipbook->Idle();
	if (GetState() == FreeLancerState::PreMove)
		GetWorldTimerManager().SetTimer(moveTimer, this, &AEnochFreeLancer::MoveTick, 0.01f, true);
	else if (GetState() == FreeLancerState::PreJump)
		GetWorldTimerManager().SetTimer(moveTimer, this, &AEnochFreeLancer::JumpTick, 0.01f, true);
}

void AEnochFreeLancer::Idle()
{
	Flipbook->Idle();
}

void AEnochFreeLancer::Attack()
{
	auto x = GetData()->direction.y;
	if (x != 0) {
		Flipbook->SetVisibility(false);
		Flipbook->SetRelativeRotationExact(UKismetMathLibrary::MakeRotFromX(FVector(0, x > 0 ? 90 : -90, 0)));
		Flipbook->SetVisibility(true);
	}
	Flipbook->Attack();
}

void AEnochFreeLancer::Victory()
{
	GetWorldTimerManager().ClearTimer(moveTimer);
	Flipbook->Victory();
}

void AEnochFreeLancer::Dead()
{
	GetWorldTimerManager().ClearTimer(moveTimer);
	UnifiedUpperUIComp->SetVisibility(false);
	Flipbook->Dead();
}

void AEnochFreeLancer::Skill()
{
	Attack();
}

void AEnochFreeLancer::MoveTick()
{
	if (GetWorldTimerManager().GetTimerElapsed(moveTimer) > 0.5)
		GetWorldTimerManager().ClearTimer(moveTimer);
	auto data = GetData();
	if (data == nullptr) return;
	auto loc = data->GetMove().floatLocation;
	auto convertedLoc = AEnochField::GetLocation(loc.x, loc.y);
	SetActorLocation(convertedLoc);
}

void AEnochFreeLancer::JumpTick()
{
	if (GetWorldTimerManager().GetTimerElapsed(moveTimer) > 0.5)
		GetWorldTimerManager().ClearTimer(moveTimer);
	auto data = GetData();
	if (data == nullptr) return;
	auto loc = data->GetJump().floatLocation;
	auto convertedLoc = AEnochField::GetLocation(loc.x, loc.y);
	SetActorLocation(convertedLoc);
}

void AEnochFreeLancer::SetActive(bool active)
{
	AEnochActor::SetActive(active);
	auto data = GetData();
	if (data == nullptr) return;
	data->BeginPlay();
}

void AEnochFreeLancer::SetDragDetectOnOff(bool OnOff)
{
	DragDetectWidgetComp->InitWidget();
	if (DragDetectWidgetComp != nullptr && DragDetectWidgetComp->GetWidget() != nullptr)
	{
		UFieldDragDetector* DragDetectWidget = Cast<UFieldDragDetector>(DragDetectWidgetComp->GetWidget());
		if (DragDetectWidget->GetIsEnabled())
		{
			if (OnOff)
				DragDetectWidget->SetVisibility(ESlateVisibility::Visible);
			else
				DragDetectWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
void AEnochFreeLancer::SetImageVisualOnOff(bool OnOff)
{
	static FVector DefaultLocation =  AnimGroupComponent->GetRelativeLocation();
	FVector Location = AnimGroupComponent->GetRelativeLocation();
	if(OnOff)
		Location = DefaultLocation;
	else
		Location.Y -=1000;
	AnimGroupComponent->SetRelativeLocation(Location);
}
