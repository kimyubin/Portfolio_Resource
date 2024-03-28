// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include <memory>
#include "EnochGeometry.h"

using namespace std;
/**
 * 하나의 필드에 대한 데이터
 */

class EnochFieldData
{
public:
	EnochFieldData();
	~EnochFieldData();
	void InitData(int heightIn, int widthIn);	//BeginPlay에서 호출
	static class EnochFieldCellData *GetData(Vector2D location);
	static int GetWidth() { return width; }
	static int GetHeight() { return height; }
	void EndPlay();
	

private:
	static unique_ptr<vector<vector<class EnochFieldCellData>>> data;
	friend class EnochSimulator;
	static int width;
	static int height;
};