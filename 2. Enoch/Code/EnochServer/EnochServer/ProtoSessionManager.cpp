#include <string>
#include "EnochServer.h"

ProtoSessionManager::ProtoSessionManager() {
	InitializeSRWLock(&sessionMapLock);
}


Session *ProtoSessionManager::createSession(SOCKET s, SOCKADDR_IN& sAddr) {

	LONGLONG id = InterlockedIncrement64(&lastSessionId);
	Session* session = Session::newSession(s, sAddr, id);
	InterlockedAdd((LONG*)&session->GetIOCount(), 1);

	return session;
}

void ProtoSessionManager::inputSession(Session& session) {
	AcquireSRWLockExclusive(&sessionMapLock);
	sessionMap.insert({ session.GetID(), &session });
	ReleaseSRWLockExclusive(&sessionMapLock);
}

Session *ProtoSessionManager::getSession(LONGLONG sessionId, boolean isLock) {
	Session* session = NULL;
	AcquireSRWLockShared(&sessionMapLock); //TODO exclusive일 필요가 있을까?
	auto iter = sessionMap.find(sessionId);
	if (iter != sessionMap.end()) {
		session = (*iter).second;
	}
	ReleaseSRWLockShared(&sessionMapLock);

	if (isLock) {
		session->Lock();
	}

	return session;
}

void ProtoSessionManager::removeSession(Session& session) {
	session.Lock(); // 삭제 이전에는 세션을 잠금

	AcquireSRWLockExclusive(&sessionMapLock);
	sessionMap.erase(session.GetID());
	ReleaseSRWLockExclusive(&sessionMapLock);

	//세션 리무브에서는 세션을 실제로 삭제시켜주지 않음, 
	// -> 세션 삭제는 후처리가 끝난 후에 해야함
}

void ProtoSessionManager::deleteSession(Session& session) {
	Session::delSession(&session);
}