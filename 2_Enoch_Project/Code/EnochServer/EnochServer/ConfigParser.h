#ifndef __CONFIG_PARSER__
#define __CONFIG_PARSER__

class ConfigParser
{
private:
	char* spaceSkip(char *c);
public:
	ConfigParser();
	~ConfigParser();
	bool init(const char* filename);
	bool containsKey(const std::string key);
	int intValue(std::string key);
	long longValue(std::string key);
	std::string stringValue(std::string key);
private:
	FILE* file;
	std::map<std::string, std::string> configMap;
};

#endif // !__CONFIG_PARSER__