#include <iostream>
#include "TCPMultiServer.h"

using namespace std;


// ����ʵ�����´��룺
// Server�����
int main()
{
	TCPMultiServer* server = new TCPMultiServer();
	if (server->Open() == false) //����socket
		return -1;
	if (server->SetIOType(TCPMultiServer::kBlocking) == false) //��������
		return -1;
	if (server->Listen(8888) == SOCKET_ERROR) //�����˿�8888
		return -1;
	server->Create_Thread();

	return 0;
}