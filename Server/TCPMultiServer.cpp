#include "TCPMultiServer.h"



TCPMultiServer::TCPMultiServer()
{
	memset((void*)&fClient, 0, sizeof(fClient));//对较大的结构体或数组进行清零操作的一种最快方法
	memset((void*)&fClient_two, 0, sizeof(fClient_two));
	hThread[2] = { 0 };

	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup的功能是初始化Winsock DLL，
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
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return false;
	}
	return true;
}

bool TCPMultiServer::Open()
{
	fSeversocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == fSeversocket)//INVALID_SOCKET  0  是无效套接字
	{
		cout << "socket failed!" << endl;
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return false;
	}
	return true;
}

bool TCPMultiServer::Close()
{
	CloseHandle(hThread[0]); //关闭一个内核对象。其中包括文件、文件映射、进程、线程、安全和同步对象等
	CloseHandle(hThread[1]);

	closesocket(fSeversocket);	//关闭sServer（服务器）IP，端口连接
	map<SOCKET, ClientInfo > ::iterator map_it = fClientMap.begin();
	while (true)
	{
		if (map_it != fClientMap.end())
			break;
		closesocket(map_it->first);	//关闭sClient（客户端）IP，端口连接
		map_it++;
	}
	WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
	system("pause");
	return true;
}

int TCPMultiServer::Listen(const unsigned short& inPort)
{
	sockaddr_in addrServ;//创建存储地址和端口的结构体
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(inPort);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retVal_r = bind(fSeversocket, (const struct sockaddr*)&addrServ, sizeof(addrServ));
	if (retVal_r == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "bind failed!" << endl;
		closesocket(fSeversocket);//关闭sServer（服务器）IP，端口连接
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	retVal_r = listen(fSeversocket, 1);
	if (retVal_r == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "listen failed!" << endl;
		closesocket(inPort);//关闭sServer连接
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
}

int TCPMultiServer::Send()
{
	Sleep(10);
	GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
	char sDateTime[30] = { 0 };
	sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d 发送成功", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	retVal_r = send(fClient_two.fSocket, sDateTime, strlen(sDateTime), 0);
	if (SOCKET_ERROR == retVal_r)		//SOCKET_ERROR 0 接口错误
	{
		int err = WSAGetLastError();	//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
		if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 资源暂时不可用
		{
			Sleep(100);
		}
		else
		{
			cout << "send failed!" << endl;
			closesocket(fSeversocket);	//关闭sServer（服务器）IP，端口连接
			closesocket(fClient_two.fSocket);	//关闭sClient（客户端）IP，端口连接
			WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
			return -1;
		}
	}
	return 0;
}



int TCPMultiServer::Recv()
{
	char buffer[TCPMultiServer::kMaxRecvBufSize];
	bool information = true;
	memset(buffer, 0, sizeof(buffer));//对较大的结构体或数组进行清零操作的一种最快方法
	map<SOCKET, ClientInfo>::iterator mclien_socket = fClientMap.begin();
	while (mclien_socket != fClientMap.end())
	{
		ZeroMemory(buffer, strlen(buffer));
		fClient_two = mclien_socket->second;
		retVal_r = recv(fClient_two.fSocket, buffer, kMaxRecvBufSize, 0);
		if (SOCKET_ERROR == retVal_r)		//SOCKET_ERROR 0 接口错误
		{
			int err = WSAGetLastError();	//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 资源暂时不可用
			{
				Sleep(100);
				mclien_socket++;
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)//WSAETIMEDOUT：10060 - 连接超时    WSAENETDOWN：10050 - 网络断开
			{
				cout << "recv failed!" << endl;
				closesocket(fSeversocket);	//关闭与（服务器）IP，端口的连接
				closesocket(fClient_two.fSocket);	//关闭与（客户端）IP，端口的连接
				fClientMap.erase(fClient_two.fSocket);
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
				return -1;
			}
		}
		else
			break;
		Sleep(10);
	}
	if (retVal_r>0)
	{
		GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
		char sDateTime[30] = { "0" };
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		cout << sDateTime << "接收到" << inet_ntoa(fClient_two.fAddress.sin_addr) << "  端口：" << ntohs(fClient_two.fAddress.sin_port)
			<< "传来的 " << retVal_r << " 字节数据: " << buffer << endl;
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
			if (INVALID_SOCKET == fClient.fSocket)//INVALID_SOCKET 是无效套接字
			{
				int err = WSAGetLastError();
				if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 资源暂时不可用
				{
					Sleep(100);
					continue;
				}
				else
				{
					cout << "accept failed!" << endl;
					closesocket(fSeversocket);//关闭（服务器）IP，端口连接
					WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
					return -1;
				}
			}
			else
				break;
		}
		GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
		char sDateTime[30];
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		fClient.fArriveTime = sDateTime;
		cout << fClient.fArriveTime << "有客户端IP:" << inet_ntoa(fClient.fAddress.sin_addr) <<
			"  端口：" << ntohs(fClient.fAddress.sin_port) << " 连接服务器！ " << endl;

		fClientMap.insert(map<SOCKET, ClientInfo>::value_type(fClient.fSocket, fClient));
		Sleep(10);
		judge_j = true;
	}
	return 0;
}


void TCPMultiServer::Create_Thread()
{
	hThread[0] = CreateThread(NULL, 0, ThreadFun_accept, this, 0, NULL); //函数功能：创建线程
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

	
