#include <Windows.h>
#include "Packet.h"

Packet::Packet()
	: err(PacketError::E_NOERROR), front(0), rear(0), size(DEFAULT_PACKET_SIZE)
{
	buf = new char[DEFAULT_PACKET_SIZE];
}

Packet::Packet(int iBufferSize)
	: err(PacketError::E_NOERROR), front(0), rear(0), size(iBufferSize)
{
	buf = new char[iBufferSize];
}

Packet::~Packet()
{
	delete buf;
}

void Packet::Release(void)
{
	front = 0;
	rear = 0;
}

void Packet::Clear()
{
	front = 0;
	rear = 0;
}

int Packet::MoveWritePos(int iSize)
{
	if (rear + iSize < size)
		rear += iSize;
	else
	{
		iSize = size - rear - 1;
		rear = size - 1;
	}

	return iSize;
}
int Packet::MoveReadPos(int iSize)
{
	if (front + iSize <= rear)
		front += iSize;
	else
	{
		iSize = rear - front;
		front = rear;
	}

	return iSize;
}

Packet* Packet::newPacket()
{
	//TODO 추후 메모리풀 추가시 메모리풀에서 꺼내도록 조치
	return new Packet();
}

void Packet::delPacket(Packet* packet)
{
	delete packet;
}

int Packet::GetData(char* chpDest, int iSize)
{
	int useSize = GetDataSize();
	if (useSize >= iSize)
		useSize = iSize;

	memcpy_s(chpDest, iSize, GetBufferPtr(), useSize);

	MoveReadPos(useSize);

	return useSize;
}

int Packet::PutData(char* chpSrc, int iSrcSize)
{
	int freeSize = GetBufferSize() - rear;
	if (freeSize < iSrcSize)
	{
		err = PacketError::E_PUTDATA_ERROR;
		return -1;
	}

	memcpy(GetBufferPtr() + rear, chpSrc, iSrcSize);
	rear += iSrcSize;

	return iSrcSize;
}

Packet& Packet::operator = (Packet& clSrcPacket)
{
	delete buf;
	buf = new char[clSrcPacket.size];
	size = clSrcPacket.size;
	memcpy(buf, clSrcPacket.buf, clSrcPacket.size);
	err = clSrcPacket.err;
	front = clSrcPacket.front;
	rear = clSrcPacket.rear;

	return *this;
}

Packet& Packet::operator << (char chValue)
{
	if (rear + sizeof(char) < size)
	{
		memcpy(buf + rear, &chValue, sizeof(char));
		MoveWritePos(sizeof(char));
	}
	else
	{
		err = PacketError::E_PUTDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator << (short shValue)
{
	if (rear + sizeof(short) < size)
	{
		memcpy(buf + rear, &shValue, sizeof(short));
		MoveWritePos(sizeof(short));
	}
	else
	{
		err = PacketError::E_PUTDATA_ERROR;
	}

	return *this;
}


Packet& Packet::operator << (int iValue)
{
	if (rear + sizeof(int) < size)
	{
		memcpy(buf + rear, &iValue, sizeof(int));
		MoveWritePos(sizeof(int));
	}
	else
	{
		err = PacketError::E_PUTDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator << (float fValue)
{
	if (rear + sizeof(float) < size)
	{
		memcpy(buf + rear, &fValue, sizeof(float));
		MoveWritePos(sizeof(float));
	}
	else
	{
		err = PacketError::E_PUTDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator << (__int64 iValue)
{
	if (rear + sizeof(__int64) < size)
	{
		memcpy(buf + rear, &iValue, sizeof(__int64));
		MoveWritePos(sizeof(__int64));
	}
	else
	{
		err = PacketError::E_PUTDATA_ERROR;
	}

	return *this;
}
Packet& Packet::operator << (double dValue)
{
	if (rear + sizeof(double) < size)
	{
		memcpy(buf + rear, &dValue, sizeof(double));
		MoveWritePos(sizeof(double));
	}
	else
	{
		err = PacketError::E_PUTDATA_ERROR;
	}

	return *this;
}


Packet& Packet::operator >> (char& chValue)
{
	if (front + sizeof(char) <= rear)
	{
		memcpy(&chValue, buf + front, sizeof(char));
		MoveReadPos(sizeof(char));
	}
	else
	{
		err = PacketError::E_GETDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator >> (short& shValue)
{
	if (front + sizeof(short) <= rear)
	{
		memcpy(&shValue, buf + front, sizeof(short));
		MoveReadPos(sizeof(short));
	}
	else
	{
		err = PacketError::E_GETDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator >> (int& iValue)
{
	if (front + sizeof(int) <= rear)
	{
		memcpy(&iValue, buf + front, sizeof(int));
		MoveReadPos(sizeof(int));
	}
	else
	{
		err = PacketError::E_GETDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator >> (float& fValue)
{
	if (front + sizeof(float) <= rear)
	{
		memcpy(&fValue, buf + front, sizeof(float));
		MoveReadPos(sizeof(float));
	}
	else
	{
		err = PacketError::E_GETDATA_ERROR;
	}

	return *this;
}

Packet& Packet::operator >> (__int64& iValue)
{
	if (front + sizeof(__int64) <= rear)
	{
		memcpy(&iValue, buf + front, sizeof(__int64));
		MoveReadPos(sizeof(__int64));
	}
	else
	{
		err = PacketError::E_GETDATA_ERROR;
	}

	return *this;
}
Packet& Packet::operator >> (double& dValue)
{
	if (front + sizeof(double) <= rear)
	{
		memcpy(&dValue, buf + front, sizeof(double));
		MoveReadPos(sizeof(double));
	}
	else
	{
		err = PacketError::E_GETDATA_ERROR;
	}

	return *this;
}