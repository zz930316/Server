#include <iostream>
#include "TCPMultiServer.h"

using namespace std;


// 可以实现如下代码：
// Server服务端
int main()
{
	TCPMultiServer* server = new TCPMultiServer();
	if (server->Open() == false) //生成socket
		return -1;
	if (server->SetIOType(TCPMultiServer::kBlocking) == false) //设置阻塞
		return -1;
	if (server->Listen(8888) == SOCKET_ERROR) //监听端口8888
		return -1;
	server->Create_Thread();

	return 0;
}