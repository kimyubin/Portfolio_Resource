#include "EnochServer.h"

LogUtil* LogUtil::_logUtil = nullptr;

LogUtil::LogUtil()
	:_logLevel(LOG_LEVEL::LOG_DEBUG), _logCount(1)
{
	sprintf(_directory, ".");
}

LogUtil* LogUtil::GetInstance()
{
	if (_logUtil == nullptr)
	{
		_logUtil = new LogUtil();
	}

	return _logUtil;
}

void LogUtil::setDirectory(const char* directory)
{
	size_t len;
	len = strlen(directory);

	if (len < MAX_PATH) {
		strcpy(_directory, directory);
	}
}

void LogUtil::setLevel(LOG_LEVEL logLevel)
{
	_logLevel = logLevel;
}

void LogUtil::Log(const char* type, LOG_LEVEL LogLevel, const char* stringFormat, ...)
{
	if (LogLevel < _logLevel)
	{
		return;
	}

	va_list va;

	time_t cur_time = time(NULL);
	tm cur_tm;

	char path[MAX_PATH];
	char message[500];
	FILE* f;

	localtime_s(&cur_tm, &cur_time);
	
	sprintf(path, "%s\\%d%02d_%s.txt", _directory, cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, type);

	f = fopen(path, "w");

	if (f == NULL)
	{
		printf("fopen error %d\n", errno);
		return;
	}

	va_start(va, stringFormat);
	vsprintf(message, stringFormat, va);
	va_end(va);



	switch (LogLevel)
	{
	case LOG_LEVEL::LOG_DEBUG:
		fprintf(f, "[%d-%02d-%02d %02d:%02d:%02d / DEBUG   / %09d] %s\n", cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec, _logCount++, message);
		break;
	case LOG_LEVEL::LOG_WARNING:
		fprintf(f, "[%d-%02d-%02d %02d:%02d:%02d / WARNING / %09d] %s\n", cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec, _logCount++, message);
		break;
	case LOG_LEVEL::LOG_ERROR:
		fprintf(f, "[%d-%02d-%02d %02d:%02d:%02d / ERROR   / %09d] %s\n", cur_tm.tm_year + 1900, cur_tm.tm_mon + 1, cur_tm.tm_mday, cur_tm.tm_hour, cur_tm.tm_min, cur_tm.tm_sec, _logCount++, message);
		break;
	default:
		printf("Log Level failed\n");
		break;
	}
	fclose(f);
}