// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochActDelay.h"

void EnochActDelay::SetDelay(float input) {
	delay = input;
	reCalculate();
}

//pre/post ratio의 합이 1이 아니게 들어오는경우가 여럿 보여서, 알아서 비율 변환
void EnochActDelay::SetPreDelayRatio(float input) {
	preDelayRatio = input;
	reCalculate();
}

void EnochActDelay::SetPostDelayRatio(float input) {
	postDelayRatio = input;
	reCalculate();
}

void EnochActDelay::reCalculate() {
	preDelay = delay * (preDelayRatio / (preDelayRatio + postDelayRatio));
	postDelay = delay * (postDelayRatio / (preDelayRatio + postDelayRatio));
}