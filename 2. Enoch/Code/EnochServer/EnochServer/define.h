#ifndef __DEFINE__
#define __DEFINE__

#define PORT 6000
#define THREAD_CNT 6
#define RUNNING_CNT 3

//��� ��ġ �ٸ������� �̵�?? �ʿ�
//define�� �ִ� ������ ���� �������Ϸ� �Ѱܾ��ϱ⿡??
struct Header
{
	WORD len;
	MessageType messageType;
};

#endif // !__DEFINE__