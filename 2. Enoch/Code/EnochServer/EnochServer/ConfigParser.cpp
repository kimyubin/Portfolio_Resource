#include "EnochServer.h"

ConfigParser::ConfigParser() : file(NULL)
{
}

ConfigParser::~ConfigParser()
{
	configMap.clear();
}

bool ConfigParser::init(const char* filename)
{
	char line[256];
	std::string key;
	std::string value;
	this->file = fopen(filename, "r");

	if (file == nullptr)
	{
		return false;
	}

	while (!feof(file))
	{
		fgets(line, 256, file);

		char* c = spaceSkip(line);
		bool valueCheck = false;

		key = "";
		value = "";

		for (; *c; c++)
		{
			if (*c == ' ' || *c == '\t')
			{
				continue;
			}

			if (*c == '=') 
			{
				c++;
				valueCheck = true;
				break;
			}

			key.push_back(*c);
		}

		if (valueCheck)
		{
			for (; *c; c++)
			{
				if (*c == ' ' || *c == '\t')
				{
					continue;
				}

				if (*c == '/')
				{
					char* next = c + 1;
					if (*next == '/')
						break;
				}
				else if (*c == '\n')
				{
					break;
				}

				value.push_back(*c);
			}

			configMap.insert(std::make_pair(key, value));
		}
	}

	fclose(file);

	return true;
}

bool ConfigParser::containsKey(const std::string key)
{
	auto iter = configMap.find(key);
	
	if (iter == configMap.end())
		return false;

	return true;
}

int ConfigParser::intValue(std::string key)
{
	auto value = configMap.find(key);

	if (value != configMap.end())
	{	
		return std::stoi(value->second);
	}

	return 0;
}

long ConfigParser::longValue(std::string key)
{
	auto value = configMap.find(key);

	if (value != configMap.end())
	{
		return std::stol(value->second);
	}

	return 0;
}
std::string ConfigParser::stringValue(std::string key)
{
	auto value = configMap.find(key);

	if (value != configMap.end())
	{
		return value->second;
	}

	return "";
}

char* ConfigParser::spaceSkip(char* c)
{
	for (; *c; c++)
	{
		if (*c == ' ' || *c == '\t')
			continue;
		break;
	}
	return c;
}