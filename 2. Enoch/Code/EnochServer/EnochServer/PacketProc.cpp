#include "EnochServer.h"

Packet *Echo(LONGLONG sessionID, Packet& p)
{
	AcquireSRWLockExclusive(&playerListLock);
	Player* player = (*playerList.find(sessionID)).second;
	ReleaseSRWLockExclusive(&playerListLock);

	Packet* packet = Packet::newPacket();
	*packet = p;

	//헤더 타입을 packet에 추가??
	/*Header header;
	header.messageType = MessageType::RES_ECHO;*/

	//SendPacket(sessionID, header, p);

	return packet;
}

Packet* DBEcho(LONGLONG sessionID, Packet& p)
{
	AcquireSRWLockExclusive(&playerListLock);
	Player* player = (*playerList.find(sessionID)).second;
	ReleaseSRWLockExclusive(&playerListLock);

	TestMapper::insertTest(player->GetID());

	Packet* packet = Packet::newPacket();
	*packet = p;

	//헤더 타입을 packet에 추가??
	/*Header header;
	header.messageType = MessageType::RES_ECHO;*/

	//SendPacket(sessionID, header, p);

	return packet;
}

Packet* SelDB(LONGLONG sessionID, Packet& p)
{
	AcquireSRWLockExclusive(&playerListLock);
	Player* player = (*playerList.find(sessionID)).second;
	ReleaseSRWLockExclusive(&playerListLock);

	printf("SELDB : %s\n", TestMapper::selectTest(1));

	Packet* packet = Packet::newPacket();
	*packet = p;

	//헤더 타입을 packet에 추가??
	/*Header header;
	header.messageType = MessageType::RES_ECHO;*/

	//SendPacket(sessionID, header, p);

	return packet;
}