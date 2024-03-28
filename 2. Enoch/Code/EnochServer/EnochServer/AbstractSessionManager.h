#ifndef __ABSTRACT_SESSION_MANAGER__
#define __ABSTRACT_SESSION_MANAGER__

class AbstractSessionManager abstract {
public:
	virtual Session *createSession(SOCKET s, SOCKADDR_IN& sAddr) = 0;
	virtual void inputSession(Session& session) = 0;
	virtual Session *getSession(LONGLONG sessionId, boolean isLock = false) = 0;
	virtual void removeSession(Session &sesion) = 0;
	virtual void deleteSession(Session& session) = 0;
private:
};

#endif // !__ABSTRACT_SESSION_MANAGER__