#include "TCPServer.h"



TCPServer::TCPServer()
{
	retVal = 0;
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	{
		cout << "WSAStartup failed!" << endl;
		Close();
	}
}

TCPServer::~TCPServer()
{
	Close();
}

bool TCPServer::SetIOType(int inIOType)
{
	ABC abc;
	inIOType = abc = kNonBlocking;
	retVal = ioctlsocket(fSeversocket, FIONBIO, (u_long FAR*)&inIOType);
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "ioctlsocket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return false;
	}
	return true;
}

bool TCPServer::Open()
{
	fSeversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == fSeversocket)//INVALID_SOCKET  0  ����Ч�׽���
	{
		cout << "socket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return false;
	}
	return true;
}

bool TCPServer::Close()
{
	closesocket(fSeversocket);	//�ر�sServer����������IP���˿�����
	map<SOCKET, ClientInfo > ::iterator map_it = fClientMap.begin();
	while (true)
	{
		if (map_it != fClientMap.end())
			break;
		closesocket(map_it->first);	//�ر�sClient���ͻ��ˣ�IP���˿�����
		map_it++;
	}
	WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
	system("pause");
	return true;
}

int TCPServer::Listen(const unsigned short& inPort)
{
	sockaddr_in addrServ;//�����洢��ַ�Ͷ˿ڵĽṹ��
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(inPort);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retVal = bind(fSeversocket, (const struct sockaddr*)&addrServ, sizeof(addrServ));
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "bind failed!" << endl;
		closesocket(fSeversocket);//�ر�sServer����������IP���˿�����
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	retVal = listen(fSeversocket, 1);
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "listen failed!" << endl;
		closesocket(inPort);//�ر�sServer����
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
}

int TCPServer::Send(ClientInfo inClientInfo, const char* inData, const size_t& inSize)
{
	//char buf[kMaxTCPPacketSize];
	//ZeroMemory(buf, kMaxTCPPacketSize);
	//sprintf_s(buf, "�յ� - %s", inData);
	while (true)
	{
		retVal = send(inClientInfo.fSocket, inData, inSize, 0);
		if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 �ӿڴ���
		{
			int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
			{
				Sleep(100);
				continue;
			}
			else
			{
				cout << "send failed!" << endl;
				closesocket(fSeversocket);	//�ر�sServer����������IP���˿�����
				closesocket(inClientInfo.fSocket);	//�ر�sClient���ͻ��ˣ�IP���˿�����
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				return -1;
			}
		}
		else
			break;
	}
	Sleep(10);
	return 0;
}

int TCPServer::Recv(ClientInfo inClientInfo, char* outBuffer, const size_t inBufferSize)
{
	map<SOCKET, ClientInfo>::iterator mclien_socket = fClientMap.begin();
	while (mclien_socket != fClientMap.end())
	{
		inClientInfo.fSocket = mclien_socket->first;
		retVal = recv(inClientInfo.fSocket, outBuffer, BUFSIZ, 0);
		if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 �ӿڴ���
		{
			int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
			{
				Sleep(100);
				mclien_socket++;
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{	//WSAETIMEDOUT��10060 - ���ӳ�ʱ    WSAENETDOWN��10050 - ����Ͽ�
				cout << "recv failed!" << endl;
				closesocket(fSeversocket);	//�ر��루��������IP���˿ڵ�����
				closesocket(inClientInfo.fSocket);	//�ر��루�ͻ��ˣ�IP���˿ڵ�����
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				return -1;
			}
		}
		else
			break;
	}
	Sleep(10);
	//Send(inClientInfo, outBuffer, inBufferSize);
	return retVal;
}


ClientInfo TCPServer::Accept()
{
	int client_fAddress_len = sizeof(client.fAddress);
	while (true)
	{
		client.fSocket = accept(fSeversocket, (sockaddr FAR*)&client.fAddress, &client_fAddress_len);
		if (INVALID_SOCKET == client.fSocket)//INVALID_SOCKET ����Ч�׽���
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
			{
				Sleep(100);
				continue;
			}
			else
			{
				cout << "accept failed!" << endl;
				closesocket(client.fSocket);//�رգ���������IP���˿�����
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				continue;
			}
		}
		else
			break;
	}
	SYSTEMTIME st;			//����ṹ�����꣬�£��գ����ڣ�ʱ���֣��룬�����;
	GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
	char sDateTime[30];
	sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	client.fArriveTime = sDateTime;
	cout << client.fArriveTime << "�пͻ���IP:" << inet_ntoa(client.fAddress.sin_addr) <<
		"  �˿ڣ�" << ntohs(client.fAddress.sin_port) << " ���ӷ������� " << endl;

	fClientMap.insert(map<SOCKET, ClientInfo>::value_type(client.fSocket, client));
	Sleep(10);
	return client;
}
