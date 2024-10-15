#ifndef __RINGBUFFER__
#define __RINGBUFFER__

#define RINGBUF_DEFAULT_SIZE 50000

class RingBuffer
{
public:
	RingBuffer();
	RingBuffer(int iBufferSize);
	~RingBuffer();

private:
	int _size;
	int _front;
	int _rear;
	int _capacity;
	char* _buf;
	SRWLOCK srwLock;
	int srwCnt;

private:
	void Initial(int iBufferSize);

	bool isFull();
	bool isEmpty();

public:
	void Resize(int size);
	int GetBufferSize(void) const;

	int GetUseSize(void) const;
	int GetFreeSize(void) const;

	int DirectEnqueueSize(void) const;
	int DirectDequeueSize(void) const;

	int Enqueue(char* chpData, int iSize);
	int Dequeue(char* chpData, int size);
#ifdef __PACKET__
	int Enqueue(Packet& p);
	int Dequeue(Packet& p, int iSize);
#endif
#ifdef _XSTRING_
	int Enqueue(std::wstring& str, int iSize);
	int Dequeue(std::wstring& str, int iSize);
#endif

	int Peek(char* chpData, int size);

	bool MoveFront(int size);
	bool MoveRear(int size);
	bool MoveWritePos(int size) { return MoveRear(size); }
	bool MoveReadPos(int size) { return MoveFront(size); }

	char* GetWritePos() const;
	char* GetReadPos() const;
	char* GetBufPtr() const;

	void Lock();
	void UnLock();
};

#endif