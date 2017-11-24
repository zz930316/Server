// TcpServer.cpp : 定义控制台应用程序的入口点。  

#include <winsock2.h>  
#include <iostream>  

#pragma comment(lib,"ws2_32.lib")  
using namespace std;

#define BUF_SIZE 64  


/*	
			运行cmd  ：
netstat -nao        :			查看有哪些端口在被使用;
netstat -nao | findstr 63790  :  查找63790这个端口;
*/

int main()
{
	WSADATA wsd;
	SOCKET sServer;
	int retVal;
	char buf[BUF_SIZE];

	//初始化Socket  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup的功能是初始化Winsock DLL，
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//创建用于监听的Socket  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	addrServ.sin_port = htons(63790);/*htons用来将主机端口转换为网络字节顺序(to network short);
							htons 将无符号短整型转换为网络字节顺序;*/
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/*htonl用来将主机地址转换为网络字节顺序(to network long);
					htonl 将无符号长整形转换为网络字节顺序;
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
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	SOCKET sClient;
	//循环等待  

	while (true)
	{
		//为一个连接请求提供服务。
		//addrClient包含了发出连接请求的客户机IP地址信息；返回的新socket描述服务器与该客户机的连接
		sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == sClient)//INVALID_SOCKET 是无效套接字
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
		break;

	}
	//循环接受客户端的数据，直到客户端发送quit命令后退出  
	while (true)
	{
		/*void ZeroMemory( PVOID Destination,SIZE_T Length );
			Destination :指向一块准备用0来填充的内存区域的开始地址。
			Length :准备用0来填充的内存区域的大小，按字节来计算。
		ZeroMemory只是将指定的内存块清零。使用结构前清零，而不让结构的成员数值具有不确定性，
			就是讲buf数组中的64个元素全部赋值为0*/
		ZeroMemory(buf, BUF_SIZE);

		/*接收端发送来的数据:
			sockfd  :  接收端套接字描述符;
			buff    :  用来存放recv函数接收到的数据的缓冲区;
			nbytes  :  指明buff的长度;
			flags   :  一般置为0;
			recv(int sockfd, void *buff, size_t nbytes, int flags);*/
		retVal = recv(sClient, buf, BUFSIZ, 0);
		if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 接口错误
		{
			int err = WSAGetLastError();	//指该函数返回上次发生的网络错误,这个函数有很多种返回值 ，百度...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 资源暂时不可用
			{
				Sleep(100);
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{	//WSAETIMEDOUT：10060 - 连接超时    WSAENETDOWN：10050 - 网络断开
				
				cout << "recv failed!" << endl;
				closesocket(sServer);	//关闭与sServer（服务器）IP，端口的连接
				closesocket(sClient);	//关闭与sClient（客户端）IP，端口的连接
				WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
				return -1;
			}
		}
		//获取系统时间  
		SYSTEMTIME st;			//这个结构体存放年，月，日，星期，时，分，秒，毫秒等;
		GetLocalTime(&st);		//获取当地的当前系统日期和时间,存放在st结构体中;
		char sDateTime[30];

		//将SYSTEMTIME结构体中的时间信息存放在sDateTime数组中
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		//打印输出信息  
		printf("%s,Recv From Client [%s:%d]:%s\n", 
			sDateTime, inet_ntoa(addrClient.sin_addr), addrClient.sin_port, buf);
			/*inet_ntoa网络地址转换转点分十进制的字符串指针;*/
			
		//如果客户端发送“quit”字符串，则服务器退出  
		if (strcmp(buf, "quit") == 0)
		{	
	/*发送数据端:	sockfd :  指定发送端套接字描述符;
					buff   :  存放要发送数据的缓冲区;
					nbytes :  实际要改善的数据的字节数;
					flags  :  一般置为0;
				send(int sockfd, const void *buff, size_t nbytes, int flags);
				若无错误发生，send()返回所发送数据的总数（请注意这个数字可能小于len中所规定的大小）。
				否则的话，返回//SOCKET_ERROR 0 接口错误，应用程序可通过WSAGetLastError()获取相应错误代码。*/
			retVal = send(sClient, "quit", strlen("quit"), 0);
			break;
		}
		else
		{
			char msg[BUF_SIZE];		//自定义宏    BUF_SIZE  64
			
			//将SYSTEMTIME结构体中的时间信息存放在sDateTime数组中
			sprintf_s(msg, "Message received - %s", buf);
			while (true)
			{
				/*发送数据端:
					sockfd :  指定发送端套接字描述符;
					buff   :  存放要发送数据的缓冲区;
					nbytes :  实际要改善的数据的字节数;
					flags  :  一般置为0;
				send(int sockfd, const void *buff, size_t nbytes, int flags);
				若无错误发生，send()返回所发送数据的总数（请注意这个数字可能小于len中所规定的大小）。
				否则的话，返回//SOCKET_ERROR 0 接口错误，应用程序可通过WSAGetLastError()获取相应错误代码。*/
				retVal = send(sClient, msg, strlen(msg), 0);
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
						closesocket(sClient);	//关闭sClient（客户端）IP，端口连接
						WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
						return -1;
					}
				}
				break;
			}
		}
	}

	//释放Socket  
	closesocket(sServer);	//关闭sServer（服务器）IP，端口连接
	closesocket(sClient);	//关闭sClient（客户端）IP，端口连接
	WSACleanup();/*WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/

	system("pause");
	return 0;
}