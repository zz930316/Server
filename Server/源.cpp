#include <iostream>
#include "TCPServer.h"

using namespace std;


// ����ʵ�����´��룺
// Server�����
int main()
{
	TCPServer* server = new TCPServer();
	if (server->Open() == false) //����socket
		return -1;
	if (server->SetIOType(TCPServer::kBlocking) == false) //��������
		return -1;
	if (server->Listen(8888) == SOCKET_ERROR) //�����˿�8888
		return -1;

	// 	HANDLE hThread[2] = { 0 };
	// 	int first = 1, second = 2, third = 3;
	// 	hThread[0] = CreateThread(NULL, 0, ThreadProc1, (LPVOID)first, 0, NULL); //�������ܣ������߳�
	// 	hThread[1] = CreateThread(NULL, 0, ThreadProc2, (LPVOID)second, 0, NULL);

	ClientInfo  clientInfo = server->Accept();
	char buffer[TCPServer::kMaxRecvBufSize];
	while (true)
	{
		memset(buffer, 0, sizeof(buffer));//�Խϴ�Ľṹ�������������������һ����췽��
		int len = server->Recv(clientInfo, buffer, sizeof(buffer));
		if (len > 0)
		{
			cout << "���յ�" << inet_ntoa(clientInfo.fAddress.sin_addr) << "  " << ntohs(clientInfo.fAddress.sin_port) << "������ " << len << " �ֽ�����: "
				<< buffer << endl;
			strcpy(buffer, "���ճɹ�");
			server->Send(clientInfo, buffer, strlen(buffer));
		}
	}
	return 0;
}