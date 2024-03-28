#ifndef __PACKET__
#define __PACKET__

#define DEFAULT_PACKET_SIZE 5000

enum class PacketError { E_NOERROR = 0, E_PUTDATA_ERROR, E_GETDATA_ERROR };

class Packet
{
public:
	Packet();
	Packet(int iBufferSize);

	virtual ~Packet();

	void Release(void);
	void Clear(void);

	int GetBufferSize(void) { return size; }
	int GetDataSize(void) { return rear - front; }

	char* GetBufferPtr(void) { return buf + front; }
	int MoveWritePos(int iSize);
	int MoveReadPos(int iSize);

	static Packet* newPacket();
	static void delPacket(Packet *packet);


	// 연산자 오버로딩
	Packet& operator = (Packet& clSrcPacket);

	Packet& operator << (char chValue);

	Packet& operator << (short shValue);

	Packet& operator << (int iValue);
	Packet& operator << (float fValue);

	Packet& operator << (__int64 iValue);
	Packet& operator << (double dValue);

	Packet& operator >> (char& chValue);
	Packet& operator >> (short& shValue);
	Packet& operator >> (int& iValue);
	Packet& operator >> (float& fValue);
	Packet& operator >> (__int64& iValue);
	Packet& operator >> (double& dValue);

	int GetData(char* chpDest, int iSize);
	int PutData(char* chpSrc, int iSrcSize);

	PacketError GetLastError() const { return err; }

protected:
	PacketError err;
	char* buf;
	int size;
	int front;
	int rear;

};

#endif