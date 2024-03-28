#include "EnochServer.h"


DBConnect::DBConnect()
	:connection(NULL), result(NULL)
{
	mysql_init(&conn);
	connect();
}

DBConnect::~DBConnect()
{
	close();
}

DBConnect* DBConnect::getInstance()
{
	//임시로 쓰레드 로컬에 저장
	//-> 한 쓰레드에 여러 커넥트가 필요한 경우?
	//-> 쓰레드별 커넥트가 필요하지 않은 경우 등을 위해 커넥트풀로 변경이 필요할 것으로 보임
	
	static thread_local DBConnect dbConnect;


	return &dbConnect;
}

bool DBConnect::isConnect()
{
	if (connection != NULL)
		return true;
	return false;
}

bool DBConnect::connect()
{
	Config* config = Config::getInstance();
	connection = mysql_real_connect(&conn,
		config->getDBServer().c_str(),
		config->getDBUser().c_str(),
		config->getDBPassword().c_str(),
		config->getDBSchema().c_str(),
		config->getDBPort(),
		(char*)NULL, 0);
	if (connection == NULL)
	{
		strcpy(lastErrorMsg, mysql_error(connection));
		lastError = mysql_errno(connection);

		return false;
	}

	mysql_set_character_set(connection, "utf8");

	return true;
}

const char* DBConnect::getLastError()
{
	return lastErrorMsg;
}

unsigned int DBConnect::getLastErrNo()
{
	return lastError;
}

bool DBConnect::query(const char* queryStr, ...)
{
	va_list va;

	va_start(va, queryStr);

	vsprintf(queryString, queryStr, va);

	char* t = va_arg(va, char*);

	va_end(va);

	int result = mysql_query(connection, queryString);
	if (result != 0)
	{
		strcpy(lastErrorMsg, mysql_error(connection));
		lastError = mysql_errno(connection);

		return false;
	}


	return true;
}

MYSQL_RES* DBConnect::getResults()
{
	return mysql_store_result(connection);
}
MYSQL_ROW DBConnect::fetchRow(MYSQL_RES* res)
{
	MYSQL_ROW ret = mysql_fetch_row(res);
	if (ret == NULL)
	{
		mysql_free_result(res);
	}

	return ret;
}
MYSQL_ROW DBConnect::getRow()
{
	MYSQL_ROW ret;
	if (result == NULL)
	{
		result = mysql_store_result(connection);
	}
	ret = mysql_fetch_row(result);

	if (ret == NULL)
	{
		mysql_free_result(result);
		result = NULL;
	}

	return ret;
}

void DBConnect::freeResult()
{
	mysql_free_result(result);
	result = NULL;
}
void DBConnect::freeResult(MYSQL_RES* res)
{
	mysql_free_result(res);
}

void DBConnect::close()
{
	mysql_close(connection);
	connection = NULL;
}

int DBConnect::getInsertID()
{
	return mysql_insert_id(connection);
}