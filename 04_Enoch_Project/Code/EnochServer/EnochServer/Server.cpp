#include "EnochServer.h"

bool Server::initServer()
{
	Config::getInstance()->init();

	//protoSessionManager ����� -> �ӽ� ���ǸŴ����� ���� ������ ��
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

	Packet payload; // ���� Ǯ�� ����ϵ��� ������ �ʿ��ҵ�?

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
		//��κ� header�� ��Ŷ�� ���ļ� ��������� �� �޽����� ������ �� �ֵ��� ���� �ʿ���
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
	//���������� ���ǰ����� ������?? -> �����ϰ� ���??? ����غ����� ����
	//��ü ��������Ʈ �� -> ���ɹ��� ���� �� ����? �����ʿ�
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

	//��ü ��������Ʈ �� -> ���ɹ��� ���� �� ����? �����ʿ�
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
		//�� �ΰ����� GetQueuedCompletionStatus�� �����ϰų� �ϸ� ���õ��� ����
		//���� ���������� ���� �����ϰ� ����
		//�� ��� �ùٸ��� ���� ����� ��� �� �ִ� ������ ���� �� ����
		//�Ϲ����� ��� �ʱ�ȭ�� �����شٰ� �ؼ� ������ ������ ����
		//������ ��� ���� ���� ������ ������ �߻��� ���� �ټ� �ִ�
		transferred = 0;
		session = NULL;
		pOverlapped = NULL;


		int ret = GetQueuedCompletionStatus(hcp, &transferred, (PULONG_PTR)&session, (LPOVERLAPPED*)&pOverlapped, INFINITE);
		InterlockedAdd((LONG*)&session->GetIOCount(), 1);


		if (transferred == 0 && session == 0 && pOverlapped == 0)
		{
			InterlockedAdd((LONG*)&session->GetIOCount(), -1);
			//�Ϲ������� post�� ���� worker�����带 �����Ű�� �������� ���
			printf("000\n");
			break;
		}

		if (ret == false && pOverlapped == NULL)
		{
			InterlockedAdd((LONG*)&session->GetIOCount(), -1);

			printf("ret error\n");
			//iocp ����
			continue;
		}

		if (transferred == 0)
		{
			//�� zero����Ʈ�� send�ؼ� �������� �۵��� �����ϴ� ����� ����� ���
			//�� �κ��� �ڵ尡 �׻� ����ó���� ���� �ƴ�
			//�׷��� ������ ������� �ʴ� ���
			//����ó��

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


		//IO��Ⱑ ���°��(recv��⵵ ���� ��쿡�� ������ �����
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

		//accept ������ �������� ������ session�� �������� �����ų� �ٸ��� ����
		if (!session->RecvPost())
		{
			sessionManager->removeSession(*session);
			sessionManager->deleteSession(*session);

			continue;
		}

		//IO��Ⱑ ���°��(recv��⵵ ���� ��쿡�� ������ �����
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
