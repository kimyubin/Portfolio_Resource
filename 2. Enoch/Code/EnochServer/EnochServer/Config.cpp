#include "EnochServer.h"

bool Config::init()
{
	ConfigParser parser;
	parser.init("config.txt"); // 설정파일 임시이름

	////////////
	// 파일 설정값 가져오기
	////////////
	if (parser.containsKey("port"))
	{
		port = parser.intValue("port");
	}

	if (parser.containsKey("threadCnt"))
	{
		threadCnt = parser.intValue("threadCnt");
	}

	if (parser.containsKey("runningCnt"))
	{
		runningCnt = parser.intValue("runningCnt");
	}

	if (parser.containsKey("dbServer"))
	{
		dbServer = parser.stringValue("dbServer");
	}

	if (parser.containsKey("dbPort"))
	{
		dbPort = parser.intValue("dbPort");
	}

	if (parser.containsKey("dbUser"))
	{
		dbUser = parser.stringValue("dbUser");
	}

	if (parser.containsKey("dbPassword"))
	{
		dbPassword = parser.stringValue("dbPassword");
	}

	if (parser.containsKey("dbSchema"))
	{
		dbSchema = parser.stringValue("dbSchema");
	}

	return true;
}

Config* Config::getInstance()
{
	static Config config;

	return &config;
}
