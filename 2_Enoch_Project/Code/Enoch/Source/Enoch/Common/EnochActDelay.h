// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class EnochActDelay
{
public:
	EnochActDelay(float delayIn, float preDelayRatioIn, float postDelayRatioIn)
		: delay(delayIn), preDelayRatio(preDelayRatioIn), postDelayRatio(postDelayRatioIn) {
		SetDelay(delay);
	};

	EnochActDelay() = delete;
	float GetDelay() { return delay; }
	void SetDelay(float input);

	//선,후딜레이는 내부에서 자동 계산되므로 Getter만있음
	float GetPreDelay() { return preDelay; }
	float GetPostDelay() { return postDelay; }
	
	//Getter는 필요가 없어서 안만듬
	void SetPreDelayRatio(float input);
	void SetPostDelayRatio(float input);

private:
	float delay;
	float preDelayRatio;
	float postDelayRatio;
	float preDelay;
	float postDelay;

	void reCalculate();
};
