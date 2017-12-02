#include "TCPMultiServer.h"



TCPMultiServer::TCPMultiServer()
{
	memset((void*)&fClient, 0, sizeof(fClient));//�Խϴ�Ľṹ�������������������һ����췽��
	memset((void*)&fClient_two, 0, sizeof(fClient_two));
	hThread[2] = { 0 };

	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	{
		cout << "WSAStartup failed!" << endl;
		Close();
	}
}


TCPMultiServer::~TCPMultiServer()
{
	Close();
}

bool TCPMultiServer::SetIOType(int inIOType)
{
	ABC abc;
	inIOType = abc = kNonBlocking;
	retVal_r = ioctlsocket(fSeversocket, FIONBIO, (u_long FAR*)&inIOType);
	if (retVal_r == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "ioctlsocket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return false;
	}
	return true;
}

bool TCPMultiServer::Open()
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

bool TCPMultiServer::Close()
{
	CloseHandle(hThread[0]); //�ر�һ���ں˶������а����ļ����ļ�ӳ�䡢���̡��̡߳���ȫ��ͬ�������
	CloseHandle(hThread[1]);

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

int TCPMultiServer::Listen(const unsigned short& inPort)
{
	sockaddr_in addrServ;//�����洢��ַ�Ͷ˿ڵĽṹ��
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(inPort);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retVal_r = bind(fSeversocket, (const struct sockaddr*)&addrServ, sizeof(addrServ));
	if (retVal_r == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "bind failed!" << endl;
		closesocket(fSeversocket);//�ر�sServer����������IP���˿�����
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	retVal_r = listen(fSeversocket, 1);
	if (retVal_r == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "listen failed!" << endl;
		closesocket(inPort);//�ر�sServer����
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
}

int TCPMultiServer::Send()
{
	Sleep(10);
	GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
	char sDateTime[30] = { 0 };
	sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d ���ͳɹ�", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	retVal_r = send(fClient_two.fSocket, sDateTime, strlen(sDateTime), 0);
	if (SOCKET_ERROR == retVal_r)		//SOCKET_ERROR 0 �ӿڴ���
	{
		int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
		if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
		{
			Sleep(100);
		}
		else
		{
			cout << "send failed!" << endl;
			closesocket(fSeversocket);	//�ر�sServer����������IP���˿�����
			closesocket(fClient_two.fSocket);	//�ر�sClient���ͻ��ˣ�IP���˿�����
			WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
			return -1;
		}
	}
	return 0;
}



int TCPMultiServer::Recv()
{
	char buffer[TCPMultiServer::kMaxRecvBufSize];
	bool information = true;
	memset(buffer, 0, sizeof(buffer));//�Խϴ�Ľṹ�������������������һ����췽��
	map<SOCKET, ClientInfo>::iterator mclien_socket = fClientMap.begin();
	while (mclien_socket != fClientMap.end())
	{
		ZeroMemory(buffer, strlen(buffer));
		fClient_two = mclien_socket->second;
		retVal_r = recv(fClient_two.fSocket, buffer, kMaxRecvBufSize, 0);
		if (SOCKET_ERROR == retVal_r)		//SOCKET_ERROR 0 �ӿڴ���
		{
			int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
			{
				Sleep(100);
				mclien_socket++;
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)//WSAETIMEDOUT��10060 - ���ӳ�ʱ    WSAENETDOWN��10050 - ����Ͽ�
			{
				cout << "recv failed!" << endl;
				closesocket(fSeversocket);	//�ر��루��������IP���˿ڵ�����
				closesocket(fClient_two.fSocket);	//�ر��루�ͻ��ˣ�IP���˿ڵ�����
				fClientMap.erase(fClient_two.fSocket);
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				return -1;
			}
		}
		else
			break;
		Sleep(10);
	}
	if (retVal_r>0)
	{
		GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
		char sDateTime[30] = { "0" };
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		cout << sDateTime << "���յ�" << inet_ntoa(fClient_two.fAddress.sin_addr) << "  �˿ڣ�" << ntohs(fClient_two.fAddress.sin_port)
			<< "������ " << retVal_r << " �ֽ�����: " << buffer << endl;
	}
	Sleep(10);
	return 0;
}

int TCPMultiServer::Accept()
{
	int fclifAddr = sizeof(fClient.fAddress);
	while (true)
	{
		Sleep(10);
		while (true)
		{
			fClient.fSocket = accept(fSeversocket, (sockaddr FAR*)&fClient.fAddress, &fclifAddr);
			if (INVALID_SOCKET == fClient.fSocket)//INVALID_SOCKET ����Ч�׽���
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
					closesocket(fSeversocket);//�رգ���������IP���˿�����
					WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
					return -1;
				}
			}
			else
				break;
		}
		GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
		char sDateTime[30];
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		fClient.fArriveTime = sDateTime;
		cout << fClient.fArriveTime << "�пͻ���IP:" << inet_ntoa(fClient.fAddress.sin_addr) <<
			"  �˿ڣ�" << ntohs(fClient.fAddress.sin_port) << " ���ӷ������� " << endl;

		fClientMap.insert(map<SOCKET, ClientInfo>::value_type(fClient.fSocket, fClient));
		Sleep(10);
		judge_j = true;
	}
	return 0;
}


void TCPMultiServer::Create_Thread()
{
	hThread[0] = CreateThread(NULL, 0, ThreadFun_accept, this, 0, NULL); //�������ܣ������߳�
	hThread[1] = CreateThread(NULL, 0, ThreadFun_recv, this, 0, NULL);
	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
}

DWORD WINAPI ThreadFun_accept(LPVOID pM)
{
	TCPMultiServer* multiSer = (TCPMultiServer*)pM;
	while (true)
	{
		multiSer->Accept();
		Sleep(100);
	}
	return 0;
}

DWORD WINAPI ThreadFun_recv(LPVOID pM)
{
	TCPMultiServer* multiSer = (TCPMultiServer*)pM;
	while (true)
	{
		Sleep(100);
		while (multiSer->judge_j)
		{
			multiSer->Recv();
			if (multiSer->retVal_r>0)
				multiSer->Send();

		}
	}
	return 0;
}

	
