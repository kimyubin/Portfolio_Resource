#pragma comment(lib, "ws2_32")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <conio.h>
#include "..\..\..\EnochServer\EnochServer\MessageType.h"
#include "..\..\..\EnochServer\EnochServer\define.h"
#include "..\..\..\EnochServer\EnochServer\Packet.h"
#include "..\..\..\EnochServer\EnochServer\RingBuffer.h"

//���� �׽�Ʈ���� ���� �׽�Ʈ �ڵ���
//���� �׽�Ʈ�� �����ϵ��� �ۼ�

enum MODE {
	single = 1,
	repeat = 2
};

void singleMode();
void repeatMode(int, boolean, int, boolean);

int main() 
{
	WSADATA wsa;

	//���� ��� ��
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//���� ������ ���������� ������ �� �߰��ؼ� ����ȭ �׽�Ʈ�� �����ϵ��� �����ʿ�
	//�߰� �߰� ������ ���� ���� ������ ���� ���� ���� �׽�Ʈ�� �߰� �ʿ�
	//������� ���� ������ ���� �׽�Ʈ�� �ݺ��ϴ� �� �߰� �ʿ�
	//������ ������ ���� �� �ƴ� send�� recv ��Ʈ�� ���ؼ� �����ϴ� ��� �߰� �ʿ�

	MODE mode;
	int modeInt;
	std::thread threadAry[50];

	while (1) 
	{
		std::cout << "��� ����(1:����,2:�ڵ�) :";
		std::cin >> modeInt;

		if (modeInt == 1) 
		{
			mode = MODE::single;
			break;
		}
		else 
		{
			mode = MODE::repeat;
			break;
		}
	}

	switch (mode)
	{
	case single:
		singleMode();
		break;
	case repeat:
		int threadCnt;
		int clientCnt;
		char input;
		boolean disconnectTest;
		int disconPercent;
		boolean randomMsg;
		std::cout << "������ ����(�ִ� 50) :";
		std::cin >> threadCnt;
		std::cout << "������� Ŭ���̾�Ʈ ���� :";
		std::cin >> clientCnt;
		std::cout << "������ �׽�Ʈ ����(1:���� 0:�̽���) :";
		std::cin >> input;
		if (input == '0') 
		{
			disconnectTest = false;
		}
		else 
		{
			disconnectTest = true;
		}
		if (disconnectTest) 
		{
			std::cout << "������ ����(100�̻� �Է½� 100����) :";
			std::cin >> disconPercent;
			if (disconPercent > 100)
			{
				disconPercent = 100;
			}
			else if (disconPercent < 0)
			{
				disconPercent = 0;
			}
		}
		std::cout << "���� �޽��� ������ �׽�Ʈ ����(1:���� 0:�̽���) :";
		std::cin >> input;
		if (input == '0')
		{
			randomMsg = false;
		}
		else
		{
			randomMsg = true;
		}
		
		for (int i = 0; i < threadCnt; i++)
		{
			threadAry[i] = std::thread(repeatMode, clientCnt, disconnectTest, disconPercent, randomMsg);
		}

		while (1) {
			if (_kbhit()) {
				WCHAR key = _getch();

				if (key == ' ')
				{
					break;
				}
			}
		}

		for (int i = 0; i < threadCnt; i++)
		{
			threadAry[i].join();
		}
		break;
	default:
		break;
	}


}

void singleMode() {
	SOCKET sock;
	int retval;
	char buf[512];
	Header header;
	header.messageType = MessageType::REQ_ECHO;

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(PORT);

	sock = socket(AF_INET, SOCK_STREAM, 0);


	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
	{
		printf("error\n");
		return;
	}

	//�α��� ��Ŷ�� ���� ó��
	ZeroMemory(buf, 512);
	recv(sock, buf, 512, 0);

	while (1)
	{
		printf("send msg : ");
		scanf("%s", buf + sizeof(header));
		int len = strlen(buf + sizeof(header));
		header.len = len;
		memcpy(buf, &header, sizeof(header));

		send(sock, buf, len + sizeof(header), 0);

		ZeroMemory(buf, 512);
		recv(sock, buf, len + sizeof(header), 0);

		buf[len + sizeof(header)] = '\0';

		printf("recv msg : %s\n", buf + sizeof(header));
		ZeroMemory(buf, 512);
	}
}

void repeatMode(int clientCnt, boolean disconnectTest, int disconPercent, boolean randomMsg) {

	Header header;
	header.messageType = MessageType::REQ_ECHO;

	SOCKET* sockAry = new SOCKET[clientCnt];
}

SOCKET setTestSock() {
	SOCKET sock;
	int retval;
	char buf[512];

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(PORT);

	sock = socket(AF_INET, SOCK_STREAM, 0);


	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
	{
		printf("error\n");
		return;
	}

	//�α��� ��Ŷ�� ���� ó��
	ZeroMemory(buf, 512);
	recv(sock, buf, 512, 0);
}