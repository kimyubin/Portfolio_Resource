#ifndef __CONFIG__
#define __CONFIG__

class Config
{
public:
	bool init();
	int getPort() { return port; };
	int getThreadCnt() { return threadCnt; }
	int getRunningCnt() { return runningCnt; };
	std::string getDBServer() { return dbServer; };
	int getDBPort() { return dbPort; };
	std::string getDBUser() { return dbUser; };
	std::string getDBPassword() { return dbPassword; };
	std::string getDBSchema() { return dbSchema; };

	static Config *getInstance();
private:
	Config() {}
	~Config() {}

	int port = 6000;
	int threadCnt = 6;
	int runningCnt = 3;
	std::string dbServer = "127.0.0.1";
	int dbPort = 3306;
	std::string dbUser = "root";
	std::string dbPassword = "password";
	std::string dbSchema = "schema";
};

#endif // !__CONFIG__

