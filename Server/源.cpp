

//   在循环在频繁的代码里添加一个时间停顿可以让cpu使用率降低

  // 多线程非阻塞服务器
/*

htons = Host To Network Short
ntohs = Network To Host Short

*/

/*	
			运行cmd  ：
netstat -nao        :			查看有哪些端口在被使用;
netstat -nao | findstr 63790  :  查找63790这个端口;
*/
#include <winsock2.h>  
#include <iostream>  
#include <map>
#include <vector>
#include <string>

#include <process.h>  
#include <windows.h>  
#pragma comment(lib,"ws2_32.lib")  
using namespace std;

#define BUF_SIZE 64  

typedef map<SOCKET, sockaddr_in> map_sock;


SOCKET sServer;//服务器套接字
SOCKET sClient_1, sClient_2;//客户端套接字
vector<SOCKET> vector_socket;
int retVal;
char buf[BUF_SIZE];
map_sock ClientMap;

bool Clientaccept = false;

unsigned int __stdcall ThreadFun_accept(PVOID pM);
unsigned int __stdcall ThreadFun_recv(PVOID pM);

int main()
{
	WSADATA wsd;
	//初始化Socket  
	//MAKEWORD(1, 1)和MAKEWORD(2, 2)的区别在于，前者只能一次接收一次，不能马上发送，而后者能
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup的功能是初始化Winsock DLL，
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//创建用于监听的Socket  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		/*创建socket。AF_INET表示在Internet中通信；
	   sock_stream   是有保障的（即能保证数据正确传送到对方）面向连接的SOCKET;
	   IPPROTO_TCP 和 IPPROTO_IP代表两种不同的协议,分别代表IP协议族里面的TCP协议和IP协议
	   也可以将IPPROTO_TCP换成0, 0 代表网络协议 TCP 或者 IP  */
	if (INVALID_SOCKET == sServer)//INVALID_SOCKET  0  是无效套接字
	{
		cout << "socket failed!" << endl;
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	//设置Socket为非阻塞模式  
	int iMode = 1;
	/*int ioctlsocket( SOCKET s, long cmd, u_long FAR *argp );
	s：一个标识套接口的描述字;		cmd：对套接口s的操作命令;		argp：指向cmd命令所带参数的指针;*/
	retVal = ioctlsocket(sServer, FIONBIO, (u_long FAR*)&iMode); //FIONBIO：将socket设置为非阻塞
																//u_long 无符号长整型 4个字节
	//当ioctlsocket函数成功后，ioctlsocket()返回0。否则的话，返回SOCKET_ERROR错误，
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "ioctlsocket failed!" << endl;
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	//设置服务器Socket地址  
	sockaddr_in addrServ;//创建存储地址和端口的结构体

	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(9513);/*htons用来将主机端口转换为网络字节顺序(to network short);
							htons 将  无符号短整型  转换为网络字节顺序;
							htons = Host To Network Short
							htonl = Host To Network long
							ntohs = Network To Host Short
							*/
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/*htonl用来将主机地址转换为网络字节顺序(to network long);
					htonl 将  无符号长整形  转换为网络字节顺序;
					htonl = Host To Network long
					htons = Host To Network Short
					ntohs = Network To Host Short
					INADDR_ANY就是指定地址为0.0.0.0的地址,表示不确定地址,或“任意地址”。”  */

	//绑定Socket //将本地地址绑定到所创建的Server上 
	//返回值表示绑定操作是否成功，0表示成功， -1表示不成功
	retVal = bind(sServer, (const struct sockaddr*)&addrServ, sizeof(sockaddr_in));

	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);//关闭sServer（服务器）IP，端口连接
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	//监听  没有错误发生，listen()返回0。否则的话，返回-1
	retVal = listen(sServer, 1);
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);//关闭sServer连接
		WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
		return -1;
	}
	//接受客户请求  
	cout << "TCP服务器启动……" << endl;

	//创建线程
	HANDLE hThread[2] = { 0 };
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun_accept, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun_recv, NULL, 0, NULL);
	//循环接受客户端的数据，直到客户端发送quit命令后退出  
	

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);//多线程控制等待
	CloseHandle(hThread[0]); //关闭一个内核对象。其中包括文件、文件映射、进程、线程、安全和同步对象等
	CloseHandle(hThread[1]);
	 //释放Socket  
	closesocket(sServer);	//关闭sServer（服务器）IP，端口连接
	map_sock::iterator map_it = ClientMap.begin();
	while (true)
	{
		if (map_it != ClientMap.end())
			break;
		closesocket(map_it->first);	//关闭sClient（客户端）IP，端口连接
		map_it++;
	}
	WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/

	system("pause");
	return 0;
}




//子线程函数  
unsigned int __stdcall ThreadFun_accept(PVOID pM)
{
	while (true)
	{
		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);
		sClient_1 = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == sClient_1)//INVALID_SOCKET 是无效套接字
		{
			int err = WSAGetLastError(); //指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
			if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 资源暂时不可用
			{
				Sleep(100);
				continue;
			}

			else
			{
				cout << "accept failed!" << endl;
				closesocket(sServer);//关闭sServer（服务器）IP，端口连接
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
				return -1;
			}
		}
		ClientMap.insert(map_sock::value_type(sClient_1, addrClient));
		//获取系统时间  
		SYSTEMTIME st;			//这个结构体存放年，月，日，星期，时，分，秒，毫秒等;
		GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
		char sDateTime[30];
		//将SYSTEMTIME结构体中的时间信息存放在sDateTime数组中
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		cout << sDateTime << "有客户端IP:" << inet_ntoa(ClientMap[sClient_1].sin_addr) <<
			"  端口：" << ntohs(ClientMap[sClient_1].sin_port) << " 连接服务器！ " << endl;
/*		ntohs 将  无符号短整型  转换为网络字节顺序;
		htons = Host To Network Short
		htonl = Host To Network long
		ntohs = Network To Host Short*/
			
		Clientaccept = true;
		Sleep(10);
	}
	return 0;
}

unsigned int __stdcall ThreadFun_recv(PVOID pM)
{
	while (true)
	{
		while (Clientaccept)
		{
			bool judge = false;
			ZeroMemory(buf, BUF_SIZE);
			while(true)
			{
				if (judge)
				{
					judge = false;
					break;
				}
				map_sock::iterator map_it = ClientMap.begin();
				while (map_it != ClientMap.end())
				{
					sClient_2 = map_it->first;
					retVal = recv(sClient_2, buf, BUFSIZ, 0);
					if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 接口错误
					{
						int err = WSAGetLastError();	//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
						if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 资源暂时不可用
						{
							Sleep(100);
							map_it++;
							continue;
						}
						else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
						{	//WSAETIMEDOUT：10060 - 连接超时    WSAENETDOWN：10050 - 网络断开

							cout << "recv failed!" << endl;
							closesocket(sServer);	//关闭与sServer（服务器）IP，端口的连接
							closesocket(sClient_2);	//关闭与sClient（客户端）IP，端口的连接
							WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
							return -1;
						}
					}
					else
					{
						judge = true;
						break;
					}
				}
				Sleep(10);
			}
			//获取系统时间  
			SYSTEMTIME st;			//这个结构体存放年，月，日，星期，时，分，秒，毫秒等;
			GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
			char sDateTime[30];
			sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			cout << sDateTime << "接收到从客户端IP:" << inet_ntoa(ClientMap[sClient_2].sin_addr) <<
				"  端口：" << ntohs(ClientMap[sClient_2].sin_port) << "  发来的 : " << buf << endl;
			//如果客户端发送“quit”字符串，则服务器退出  
 			if (strcmp(buf, "quit") == 0)
 			{
 				retVal = send(sClient_2, "quit", strlen("quit"), 0);
 				break;
 			}
 			else
 			{
 				char msg[BUF_SIZE];		//自定义宏    BUF_SIZE  64
 										//将SYSTEMTIME结构体中的时间信息存放在sDateTime数组中
 				sprintf_s(msg, "Message received - %s", buf);
 				while (true)
 				{
 					retVal = send(sClient_2, msg, strlen(msg), 0);
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
 							closesocket(sServer);	//关闭sServer（服务器）IP，端口连接
 							closesocket(sClient_2);	//关闭sClient（客户端）IP，端口连接
 							WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
 							return -1;
 						}
 					}
 					break;
 				}
			}
		}
		Sleep(10);
	}
	return 0;
}