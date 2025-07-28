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
	AcquireSRWLockShared(&sessionMapLock); //TODO exclusive�� �ʿ䰡 ������?
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
	session.Lock(); // ���� �������� ������ ���

	AcquireSRWLockExclusive(&sessionMapLock);
	sessionMap.erase(session.GetID());
	ReleaseSRWLockExclusive(&sessionMapLock);

	//���� �����꿡���� ������ ������ ������������ ����, 
	// -> ���� ������ ��ó���� ���� �Ŀ� �ؾ���
}

void ProtoSessionManager::deleteSession(Session& session) {
	Session::delSession(&session);
}