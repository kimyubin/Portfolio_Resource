#include "EnochServer.h"

int main()
{
	Server* server = new Server();
	server->initServer();
	server->InitContents();
	//initNetwork();
	//InitContents();

	//TODO 모니터링출력
	while (1)
	{
		//system("pause");
	}
}