#include "EnochServer.h"

int TestMapper::insertTest(long num)
{
	DBConnect *connect = DBConnect::getInstance();

	//���⼭�� ���� �� Ʈ����� �õ� -> �����δ� Ʈ������� �Ͼ�� ���� ���缭 ���� ���ᰡ �ʿ�
	connect->query("BEGIN"); //TODO ���� ���� Ʈ������� ���ϰ� �� �� �ֵ��� ����??
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
	char * ret = row[0]; // ����� �� �ڷ������� ������ �� �ִ� parser �߰� �ʿ��� ��?

	connect->freeResult();

	return ret;
}
