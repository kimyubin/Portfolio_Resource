#ifndef __DB_CONNECT__
#define __DB_CONNECT__

class DBConnect
{
public:
	static DBConnect* getInstance();
	bool connect();
	const char* getLastError();
	unsigned int getLastErrNo();
	bool query(const char* query, ...);
	MYSQL_RES* getResults();
	MYSQL_ROW fetchRow(MYSQL_RES* res);
	MYSQL_ROW getRow();
	void freeResult();
	void freeResult(MYSQL_RES*);
	void close();
	int getInsertID();
	bool isConnect();
private:
	DBConnect();
	~DBConnect();

	MYSQL conn;
	MYSQL* connection;

	MYSQL_RES* result;

	char		queryString[2048];
	int			lastError;
	char		lastErrorMsg[128];
};

#endif // !__DB_CONNECT__