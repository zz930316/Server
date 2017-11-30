#include <iostream>
#include "TCPServer.h"

using namespace std;


// 可以实现如下代码：
// Server服务端
int main()
{
	TCPServer* server = new TCPServer();
	if (server->Open() == false) //生成socket
		return -1;
	if (server->SetIOType(TCPServer::kBlocking) == false) //设置阻塞
		return -1;
	if (server->Listen(8888) == SOCKET_ERROR) //监听端口8888
		return -1;

	// 	HANDLE hThread[2] = { 0 };
	// 	int first = 1, second = 2, third = 3;
	// 	hThread[0] = CreateThread(NULL, 0, ThreadProc1, (LPVOID)first, 0, NULL); //函数功能：创建线程
	// 	hThread[1] = CreateThread(NULL, 0, ThreadProc2, (LPVOID)second, 0, NULL);

	ClientInfo  clientInfo = server->Accept();
	char buffer[TCPServer::kMaxRecvBufSize];
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));//对较大的结构体或数组进行清零操作的一种最快方法
		int len = server->Recv(clientInfo, buffer, sizeof(buffer));
		if (len > 0)
		{
			cout << "接收到" << inet_ntoa(clientInfo.fAddress.sin_addr) << "  " << ntohs(clientInfo.fAddress.sin_port) << "传来的 " << len << " 字节数据: "
				<< buffer << endl;
			strcpy(buffer, "接收成功");
			server->Send(clientInfo, buffer, strlen(buffer));
		}
	}
	return 0;
}