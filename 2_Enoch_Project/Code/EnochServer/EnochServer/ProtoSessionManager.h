#ifndef __PROTO_SESSION_MANAGER__
#include "AbstractSessionManager.h"

//테스트용 세션을 생성, 재활용하거나하지 않는 세션관리자
class ProtoSessionManager : public AbstractSessionManager {
public:
	Session *createSession(SOCKET s, SOCKADDR_IN& sAddr);
	void inputSession(Session& session);
	Session *getSession(LONGLONG sessionId, boolean isLock = false);
	void removeSession(Session &session);
	void deleteSession(Session& session);
	ProtoSessionManager();
private:
	LONGLONG lastSessionId = 0;
	std::map<LONGLONG, Session*> sessionMap;
	SRWLOCK sessionMapLock;
	ProtoSessionManager* instance;
};
#endif // !__PROTO_SESSION_MANAGER__
