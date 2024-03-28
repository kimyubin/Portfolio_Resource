#pragma once

#include <string>
#include <vector>

class CSVParser
{
public:
	CSVParser(std::wstring path);

	bool success;
	std::vector<std::vector<std::wstring>> content; 
};
