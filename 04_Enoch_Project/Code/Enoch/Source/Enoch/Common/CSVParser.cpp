#include "CSVParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

#define TRIM_SPACE L"\n\t\r "
inline std::wstring trim(std::wstring& s,const std::wstring& drop = TRIM_SPACE)
{
	std::wstring r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}

inline std::wstring rtrim(std::wstring s,const std::wstring& drop = TRIM_SPACE)
{
	return s.erase(s.find_last_not_of(drop)+1);
}

inline std::wstring ltrim(std::wstring s,const std::wstring& drop = TRIM_SPACE)
{ 
	return s.erase(0,s.find_first_not_of(drop)); 
}

CSVParser::CSVParser(std::wstring path)
{
	success = false;

	const int BUF_SIZE = 256;
    char cstrPath[BUF_SIZE];
	size_t size = 0;
	const int str_len = path.length() + 1;
	wcstombs_s(&size, cstrPath, str_len, path.c_str(), BUF_SIZE);

	const std::locale empty_locale = std::locale::empty();
	typedef std::codecvt_utf8<wchar_t> converter_type;
	const converter_type* converter = new converter_type;
	const std::locale utf8_locale = std::locale(empty_locale, converter);

    std::wifstream file(cstrPath);
	if(!file.is_open())
		return;
	file.imbue(utf8_locale);

	std::wstring wline;
	while( std::getline(file, wline))
	{
		content.push_back(std::vector<std::wstring>());
		auto &vec = content.back();

		std::wstringstream wss(wline);
		std::wstring tok;
		while(std::getline(wss, tok, L','))
			vec.push_back(trim(tok));
	}
	success = true;
}
