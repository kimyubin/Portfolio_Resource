#ifndef __LOG_UTIL__
#define __LOG_UTIL

enum class LOG_LEVEL { LOG_DEBUG = 0, LOG_WARNING, LOG_ERROR };

#define LOG(type,level,stringFormat,...) (logUtil->GetInstance()->Log(type,level,stringFormat,##__VA_ARGS__))
#define SET_LOG_LEVEL(level) (logUtil->GetInstance()->setLevel(level))
#define SET_LOG_DIRECTORY(string) (logUtil->GetInstance()->setDirectory(string))

class LogUtil
{
public:
	static LogUtil* GetInstance();
	void setDirectory(const char* directory);
	void setLevel(LOG_LEVEL logLevel);
	void Log(const char* szType, LOG_LEVEL LogLevel, const char* stringFormat, ...);
private:
	LogUtil();
	static LogUtil* _logUtil;
	LOG_LEVEL _logLevel;
	char _directory[MAX_PATH];
	int _logCount;
};

#endif // !__LOG_UTIL__
