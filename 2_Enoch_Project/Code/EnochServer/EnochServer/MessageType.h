#pragma once

//�ϴ� Ȧ�� REQ ¦�� RES��? -> �׽�Ʈ �ܰ� ������ ���� �����ؾ��� ��
enum class MessageType {
	DEFAULT = 0, //�׽�Ʈ��

	// 0~ 100 TEST��
	REQ_ECHO = 1,
	RES_ECHO = 2,

	REQ_DBECHO = 3,
	RES_DBECHO = 4,

	REQ_SELDB = 5,
	RES_SELDB = 6,

	// 101 ~ 200 COMMON
	RES_CLIENT_JOIN = 101,
};