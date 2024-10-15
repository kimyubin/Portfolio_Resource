#ifndef __DEFINE__
#define __DEFINE__

#define PORT 6000
#define THREAD_CNT 6
#define RUNNING_CNT 3

//헤더 위치 다른곳으로 이동?? 필요
//define에 있는 내용은 추후 설정파일로 넘겨야하기에??
struct Header
{
	WORD len;
	MessageType messageType;
};

#endif // !__DEFINE__