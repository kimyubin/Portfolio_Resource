// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochPaperFlipbookComponent.h"
#include "PaperFlipbook.h"
#include "EnochFreeLancer.h"
#include "EnochProjectile.h"


enum _state {
	IDLE = 0,
	ATTACK,
	MOVE,
	DEAD,
	VICTORY,
	FLOATING,
	BOMB,
};

void UEnochPaperFlipbookComponent::SetFreelancer(
	UPaperFlipbook *idle,
	UPaperFlipbook *attack,
	UPaperFlipbook *move,
	UPaperFlipbook *dead,
	UPaperFlipbook *victory)
{
	books.Add(IDLE, idle);
	books.Add(ATTACK, attack);
	books.Add(MOVE, move);
	books.Add(DEAD, dead);
	books.Add(VICTORY, victory);
	SetRelativeRotation(FRotator(0, 90, 0));
}

//재생동작을 변경하는 함수들
void UEnochPaperFlipbookComponent::Attack() {
	setbook(ATTACK);
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
}

void UEnochPaperFlipbookComponent::Idle() {
	setbook(IDLE);
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
}

void UEnochPaperFlipbookComponent::Move() {
	setbook(MOVE);
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
}

void UEnochPaperFlipbookComponent::Victory() {
	setbook(VICTORY);
	SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
}

void UEnochPaperFlipbookComponent::Dead() {
	setbook(DEAD, false);
	SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
	SetWorldRotation(FRotator(180,90,-90));
	//SetRelativeRotation(FRotator(0, 90, 90));
}
void UEnochPaperFlipbookComponent::SetProjectileCopy(UEnochPaperFlipbookComponent* other)
{
	if(other->books.Find(FLOATING) != nullptr)
		books.Add(FLOATING, other->books[FLOATING]);
	if (other->books.Find(BOMB) != nullptr)
		books.Add(BOMB, other->books[BOMB]);
}
void UEnochPaperFlipbookComponent::SetProjectile(
	UPaperFlipbook* floating,
	UPaperFlipbook* bomb)
{
	books.Add(FLOATING, floating);
	books.Add(BOMB, bomb);
	SetRelativeRotation(FRotator(0, 90, 0));
}

void UEnochPaperFlipbookComponent::Floating() {
	setbook(FLOATING);
}

void UEnochPaperFlipbookComponent::Bomb() {
	setbook(BOMB, false);
}

void UEnochPaperFlipbookComponent::setbook(int type, bool loop)
{
	auto book = books.Find(type);
	if (book == nullptr) return;
	Stop();
	SetLooping(loop);
	SetFlipbook(*book);
	Play();
}
