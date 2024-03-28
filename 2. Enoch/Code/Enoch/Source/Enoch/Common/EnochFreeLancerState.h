// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//모든 동작은 선딜레이, 후딜레이 상태가 구분되므로 Pre, Post로 구분
UENUM()
enum class FreeLancerState : uint8 {
	None=0,
	Idle,
	PreMove,
	PostMove,
	PreJump,
	PostJump,
	PreAttack,
	PostAttack,
	PreSkill,
	PostSkill,
	Victory,
	Dead,
};//마지막 State를 Dead로 고정할 예정