// Fill out your copyright notice in the Description page of Project Settings.


#include "EnochFieldData.h"
#include "EnochFieldCellData.h"
#include <memory>
#include <vector>

unique_ptr<vector<vector<class EnochFieldCellData>>> EnochFieldData::data;
//EnochField 비긴 플레이 초기화 -> 엔진 런타임과 별개로 초기화로 임시 변경
int EnochFieldData::width=10, EnochFieldData::height=10;

EnochFieldData::EnochFieldData()
{
}

void EnochFieldData::InitData(int heightIn, int widthIn)
{
	data = make_unique<vector<vector<EnochFieldCellData>>>();
	//height = heightIn;
	//width = widthIn;
	data->reserve(height);
	for (auto row = 0; row < height; row++) {
		data->emplace_back(vector<EnochFieldCellData>());
		data->at(row).reserve(width);
		for (auto cell = 0; cell < width; cell++) {
			data->at(row).emplace_back(EnochFieldCellData(row, cell));
		}
	}
}

EnochFieldCellData *EnochFieldData::GetData(Vector2D location)
{
	if (data == nullptr) return nullptr;
	if (location.y >= height || location.y < 0) return nullptr;
	if (location.x >= width || location.x < 0) return nullptr;
	return &(data->at(location.y)).at(location.x);
}
EnochFieldData::~EnochFieldData()
{
}

void EnochFieldData::EndPlay()
{
	data = nullptr;
}