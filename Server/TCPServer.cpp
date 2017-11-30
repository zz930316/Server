#include "TCPServer.h"



TCPServer::TCPServer()
{
	retVal = 0;
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup的功能是初始化Winsock DLL，
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
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return false;
	}
	return true;
}

bool TCPServer::Open()
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

bool TCPServer::Close()
{
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

int TCPServer::Listen(const unsigned short& inPort)
{
	sockaddr_in addrServ;//创建存储地址和端口的结构体
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(inPort);
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retVal = bind(fSeversocket, (const struct sockaddr*)&addrServ, sizeof(addrServ));
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "bind failed!" << endl;
		closesocket(fSeversocket);//关闭sServer（服务器）IP，端口连接
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	retVal = listen(fSeversocket, 1);
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "listen failed!" << endl;
		closesocket(inPort);//关闭sServer连接
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
}

int TCPServer::Send(ClientInfo inClientInfo, const char* inData, const size_t& inSize)
{
	//char buf[kMaxTCPPacketSize];
	//ZeroMemory(buf, kMaxTCPPacketSize);
	//sprintf_s(buf, "收到 - %s", inData);
	while (true)
	{
		retVal = send(inClientInfo.fSocket, inData, inSize, 0);
		if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 接口错误
		{
			int err = WSAGetLastError();	//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 资源暂时不可用
			{
				Sleep(100);
				continue;
			}
			else
			{
				cout << "send failed!" << endl;
				closesocket(fSeversocket);	//关闭sServer（服务器）IP，端口连接
				closesocket(inClientInfo.fSocket);	//关闭sClient（客户端）IP，端口连接
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
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
		if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 接口错误
		{
			int err = WSAGetLastError();	//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 资源暂时不可用
			{
				Sleep(100);
				mclien_socket++;
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{	//WSAETIMEDOUT：10060 - 连接超时    WSAENETDOWN：10050 - 网络断开
				cout << "recv failed!" << endl;
				closesocket(fSeversocket);	//关闭与（服务器）IP，端口的连接
				closesocket(inClientInfo.fSocket);	//关闭与（客户端）IP，端口的连接
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
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
		if (INVALID_SOCKET == client.fSocket)//INVALID_SOCKET 是无效套接字
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
				closesocket(client.fSocket);//关闭（服务器）IP，端口连接
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
				continue;
			}
		}
		else
			break;
	}
	SYSTEMTIME st;			//这个结构体存放年，月，日，星期，时，分，秒，毫秒等;
	GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
	char sDateTime[30];
	sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	client.fArriveTime = sDateTime;
	cout << client.fArriveTime << "有客户端IP:" << inet_ntoa(client.fAddress.sin_addr) <<
		"  端口：" << ntohs(client.fAddress.sin_port) << " 连接服务器！ " << endl;

	fClientMap.insert(map<SOCKET, ClientInfo>::value_type(client.fSocket, client));
	Sleep(10);
	return client;
}
