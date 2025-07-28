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

//에코 테스트만을 위한 테스트 코드임
//대충 테스트만 가능하도록 작성

enum MODE {
	single = 1,
	repeat = 2
};

void singleMode();
void repeatMode(int, boolean, int, boolean);

int main() 
{
	WSADATA wsa;

	//소켓 통신 부
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//연속 데이터 무지성으로 날리는 걸 추가해서 동기화 테스트도 가능하도록 수정필요
	//중간 중간 재접속 등을 통해 나갔다 들어올 때의 상태 테스트도 추가 필요
	//쓰레드로 여러 계정이 위의 테스트를 반복하는 것 추가 필요
	//응답을 눈으로 보는 게 아닌 send와 recv 스트링 비교해서 검증하는 기능 추가 필요

	MODE mode;
	int modeInt;
	std::thread threadAry[50];

	while (1) 
	{
		std::cout << "모드 고르기(1:수동,2:자동) :";
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
		std::cout << "쓰레드 갯수(최대 50) :";
		std::cin >> threadCnt;
		std::cout << "쓰레드당 클라이언트 갯수 :";
		std::cin >> clientCnt;
		std::cout << "재접속 테스트 여부(1:실행 0:미실행) :";
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
			std::cout << "재접속 비율(100이상 입력시 100고정) :";
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
		std::cout << "연속 메시지 보내기 테스트 여부(1:실행 0:미실행) :";
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

	//로그인 패킷에 대한 처리
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

	//로그인 패킷에 대한 처리
	ZeroMemory(buf, 512);
	recv(sock, buf, 512, 0);
}