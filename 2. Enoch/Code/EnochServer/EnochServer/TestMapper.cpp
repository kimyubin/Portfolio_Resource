#include "EnochServer.h"

int TestMapper::insertTest(long num)
{
	DBConnect *connect = DBConnect::getInstance();

	//여기서는 편의 상 트랜잭션 시도 -> 실제로는 트랜잭션이 일어나는 때에 맞춰서 시작 종료가 필요
	connect->query("BEGIN"); //TODO 추후 쿼리 트랜잭션을 편하게 할 수 있도록 개조??
	connect->query("INSERT INTO TEST(num) VALUES(%d)", num);
	int ret = connect->getInsertID();
	connect->query("COMMIT");

	connect->freeResult();

	return ret;
}

char * TestMapper::selectTest(long index)
{
	DBConnect *connect = DBConnect::getInstance();

	connect->query("SELECT num FROM TEST WHERE `index` = %d", index);

	MYSQL_ROW row = connect->getRow();
	char * ret = row[0]; // 결과를 각 자료형으로 변경할 수 있는 parser 추가 필요할 듯?

	connect->freeResult();

	return ret;
}
