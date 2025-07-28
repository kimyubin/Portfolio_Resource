#include "EnochServer.h"

bool Server::initServer()
{
	Config::getInstance()->init();

	//protoSessionManager 사용중 -> 임시 세션매니저니 추후 변경할 것
	sessionManager = new ProtoSessionManager();

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return false;

	hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, Config::getInstance()->getRunningCnt());

	if (hcp == NULL) return false;

	hAcceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptThreadInit, this, 0, NULL);
	if (hAcceptThread == NULL) return false;

	hWorkerThread = new HANDLE[Config::getInstance()->getThreadCnt()];

	for (int i = 0; i < Config::getInstance()->getThreadCnt(); i++)
	{
		hWorkerThread[i] = (HANDLE)_beginthreadex(NULL, 0, WorkerThreadInit, this, 0, NULL);
		if (hWorkerThread[i] == NULL) return false;
		//CloseHandle(hThread);
	}

	return true;
}

PROCRESULT Server::CompleteRecvPacket(Session* session)
{
	Header header;
	int recvQSize = session->GetRecvQ().GetUseSize();

	Packet payload; // 추후 풀을 사용하도록 수정은 필요할듯?

	if (sizeof(header) > recvQSize)
		return PROCRESULT::NONE;

	session->GetRecvQ().Peek((char*)&header, sizeof(Header));

	if (recvQSize < header.len + sizeof(header))
		return PROCRESULT::NONE;

	session->GetRecvQ().MoveReadPos(sizeof(Header));

	if (session->GetRecvQ().Dequeue(payload, header.len) != header.len)
		return PROCRESULT::FAIL;

	Packet *packet = OnRecv(session->GetID(), payload, header.messageType);

	if (packet != NULL)
	{
		Packet::delPacket(packet);
	}

	return PROCRESULT::SUCCESS;
}


bool Server::SendPacket(LONGLONG sessionID, Header header, Packet& p)
{
	
	Session *session = sessionManager->getSession(sessionID, true);


	header.len = p.GetDataSize();

	if (session->GetSendQ().GetFreeSize() >= p.GetDataSize() + sizeof(header))
	{
		session->GetSendQ().Enqueue((char*)&header, sizeof(header));
		session->GetSendQ().Enqueue(p);
	}
	else
	{
		session->Unlock();
		return false;
	}

	session->SendPost();

	session->Unlock();
	return true;
}

Packet* Server::OnRecv(LONGLONG sessionID, Packet& p, MessageType type)
{
	Packet* packet = NULL;
	Header header;
	switch (type)
	{
		//요부분 header와 패킷을 합쳐서 헤더조립도 각 메시지에 대응될 수 있도록 수정 필요함
	case MessageType::REQ_ECHO:
		packet =Echo(sessionID, p);
		header.messageType = MessageType::RES_ECHO;
		SendPacket(sessionID, header, *packet);
		break;
	case MessageType::REQ_DBECHO:
		packet = DBEcho(sessionID, p);
		header.messageType = MessageType::RES_DBECHO;
		SendPacket(sessionID, header, *packet);
		break;
	case MessageType::REQ_SELDB:
		packet = DBEcho(sessionID, p);
		header.messageType = MessageType::RES_SELDB;
		SendPacket(sessionID, header, *packet);
		break;
	}

	return packet;
}

void Server::OnClientLeave(LONGLONG sessionID)
{
	//유저관리와 세션관리를 별개로?? -> 동일하게 사용??? 고민해봐야할 문제
	//전체 유저리스트 락 -> 성능문제 있을 수 있음? 수정필요
	AcquireSRWLockExclusive(&playerListLock);
	Player* player = (*playerList.find(sessionID)).second;
	playerList.erase(playerList.find(sessionID));
	player->Lock();
	ReleaseSRWLockExclusive(&playerListLock);
	printf("player leave %lld\n", player->GetID());
	player->UnLock();
	delete player;
}

void Server::OnClientJoin(LONGLONG sessionID)
{
	Player* player = new Player(sessionID);
	Packet p;
	p << 0x7fffffffffffffff;

	//전체 유저리스트 락 -> 성능문제 있을 수 있음? 수정필요
	AcquireSRWLockExclusive(&playerListLock);
	playerList.insert(std::make_pair(sessionID, player));

	Header header;
	header.messageType = MessageType::RES_CLIENT_JOIN;

	SendPacket(sessionID, header, p);

	ReleaseSRWLockExclusive(&playerListLock);
	printf("player join %lld\n", player->GetID());
}

void Server::InitContents()
{
	InitializeSRWLock(&playerListLock);
}

unsigned int Server::WorkerThreadInit(void* param) {
	Server* server = (Server*)param;
	return server->WorkerThread();
}
unsigned int Server::AcceptThreadInit(void* param) {
	Server* server = (Server*)param;
	return server->AcceptThread();
}

bool Server::WorkerThread()
{

	Session* session;
	MyOverlapped* pOverlapped;
	DWORD transferred;

	printf("Worker thread On\n");
	while (1)
	{
		//이 두가지는 GetQueuedCompletionStatus가 실패하거나 하면 세팅되지 않음
		//이전 루프에서의 값을 보존하고 있음
		//이 경우 올바르지 않은 대상을 끊어낼 수 있는 문제가 생길 수 있음
		//일반적인 경우 초기화를 안해준다고 해서 문제가 생기진 않음
		//하지만 기능 변경 등의 문제로 문제가 발생할 위험 다소 있다
		transferred = 0;
		session = NULL;
		pOverlapped = NULL;


		int ret = GetQueuedCompletionStatus(hcp, &transferred, (PULONG_PTR)&session, (LPOVERLAPPED*)&pOverlapped, INFINITE);
		InterlockedAdd((LONG*)&session->GetIOCount(), 1);


		if (transferred == 0 && session == 0 && pOverlapped == 0)
		{
			InterlockedAdd((LONG*)&session->GetIOCount(), -1);
			//일반적으로 post를 통해 worker스레드를 종료시키는 목적으로 사용
			printf("000\n");
			break;
		}

		if (ret == false && pOverlapped == NULL)
		{
			InterlockedAdd((LONG*)&session->GetIOCount(), -1);

			printf("ret error\n");
			//iocp 오류
			continue;
		}

		if (transferred == 0)
		{
			//단 zero바이트를 send해서 동기적인 작동을 유도하는 기법을 사용할 경우
			//이 부분의 코드가 항상 끊김처리인 것은 아님
			//그러나 보통은 사용하지 않는 기법
			//끊김처리

			InterlockedExchange8((CHAR*)&session->GetSocketActive(), FALSE);
			closesocket(session->GetSocket());
			printf("transffer 0 %lu\n", (unsigned long)session->GetSocket());
		}

		if (pOverlapped->type == TYPE::RECV)
		{
			InterlockedAdd((LONG*)&session->GetIOCount(), -1);
			session->GetRecvQ().MoveWritePos(transferred);
			session->GetRecvQ().UnLock();
			while (1)
			{
				if (CompleteRecvPacket(session) != PROCRESULT::SUCCESS)
					break;
			}
			session->RecvPost();
		}
		else if (pOverlapped->type == TYPE::SEND)
		{
			InterlockedAdd((LONG*)&session->GetIOCount(), -1);
			session->GetSendQ().MoveReadPos(transferred);
			InterlockedExchange8(&session->GetSendFlag(), 1);
			session->SendPost();
		}


		//IO대기가 없는경우(recv대기도 없는 경우에는 세션이 종료됨
		if (InterlockedAdd((LONG*)&session->GetIOCount(), -1) == 0)
		{
			printf("delete session %lld\n", session->GetID());

			sessionManager->removeSession(*session);

			OnClientLeave(session->GetID());
			session->Unlock();
			
			sessionManager->deleteSession(*session);
		}
	}


	return true;
}

bool Server::AcceptThread()
{
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSock == INVALID_SOCKET)
	{
		printf("socket error\n");
		return false;
	}

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(Config::getInstance()->getPort());
	int retval = bind(listenSock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR)
	{
		printf("bind error\n");
		return false;
	}

	retval = listen(listenSock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		printf("listen error\n");
		return false;
	}

	//Session* session;
	SOCKET sock;
	SOCKADDR_IN sockAddr;
	int addrLen;
	//long long num = 0;

	//InitializeSRWLock(&sessionListLock);
	printf("Accept thread On\n");

	while (1)
	{
		addrLen = sizeof(sockAddr);
		sock = accept(listenSock, (SOCKADDR*)&(sockAddr), &addrLen);
		if (sock == INVALID_SOCKET)
		{
			printf("accept error\n");
			continue;
		}

		int optval = 0;
		retval = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));
		if (retval == SOCKET_ERROR)
		{
			printf("SO_RCVBUF error\n");
			continue;
		}

		Session *session = sessionManager->createSession(sock, sockAddr);

		CreateIoCompletionPort((HANDLE)sock, hcp, (ULONG_PTR)session, 0);

		sessionManager->inputSession(*session);
		OnClientJoin(session->GetID());

		//accept 순간에 성공하지 않으면 session이 생성되지 않은거나 다름이 없음
		if (!session->RecvPost())
		{
			sessionManager->removeSession(*session);
			sessionManager->deleteSession(*session);

			continue;
		}

		//IO대기가 없는경우(recv대기도 없는 경우에는 세션이 종료됨
		if (InterlockedAdd((LONG*)&session->GetIOCount(), -1) == 0)
		{
			printf("delete session %lld\n", session->GetID());

			sessionManager->removeSession(*session);

			OnClientLeave(session->GetID());
			session->Unlock();

			sessionManager->deleteSession(*session);
		}

	}

	return true;
}
