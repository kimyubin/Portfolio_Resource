#ifndef __SERVER__
#define __SERVER__


enum class PROCRESULT { SUCCESS = 0, NONE, FAIL };

class Server {
public:
	bool initServer();
	PROCRESULT CompleteRecvPacket(Session* session);
	bool SendPacket(LONGLONG sessionID, Header header, Packet& p);

	Packet* OnRecv(LONGLONG sessionID, Packet& p, MessageType type = MessageType::DEFAULT);

	void OnClientLeave(LONGLONG sessionID);

	void OnClientJoin(LONGLONG sessionID);

	void InitContents();

	static unsigned int WorkerThreadInit(void * param);
	static unsigned int AcceptThreadInit(void * param);

	bool WorkerThread(void);
	bool AcceptThread(void);

private:
	HANDLE hcp;
	HANDLE hAcceptThread;
	HANDLE *hWorkerThread;
	AbstractSessionManager *sessionManager; // ���ǿ� ���� ������ ���������� �������(�̱�������??? ����??)
};


#endif //__SERVER__