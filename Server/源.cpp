#include <iostream>  
#include <Winsock2.h> //windows socket的头文件  

#pragma comment( lib, "ws2_32.lib" )// 链接Winsock2.h的静态库文件  
using namespace std;

void main()
{
	//初始化winsocket  
	WORD wVersionRequested;
	WSADATA wsaData;

	//对winsock DLL（动态链接库文件）进行初始化，协商Winsock的版本支持，并分配必要的资源。
	//MAKEWORD(1, 1)和MAKEWORD(2, 2)的区别在于，前者只能一次接收一次，不能马上发送，而后者能
	WSAStartup(MAKEWORD(1, 1), &wsaData);

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);	//创建socket。AF_INET表示在Internet中通信；
														//SOCK_STREAM表示socket是流套接字，对应tcp；
														//0指定网络协议为TCP/IP  
	SOCKADDR_IN addrSrv;

						/*htonl用来将主机地址转换为网络字节顺序(to network long);
					htonl 将无符号长整形转换为网络字节顺序;*/
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY就是指定地址为0.0.0.0的地址,表示不确定地址,或“任意地址”。”  
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4000);/*htons用来将主机端口转换为网络字节顺序(to network short);
							htons 将无符号短整型转换为网络字节顺序;*/

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//将本地地址绑定到所创建的socket上，以使在网络上标识该socket

	listen(sockSrv, 5);//socket监听，准备接受客户端通过 connect()发出的连接请求。  
					   //listen()函数不会阻塞，它主要做的事情为，将该套接字和套接字对应的连接队列长度告诉 Linux 内核，然后，listen()函数就结束。
					   //只要 TCP 服务器调用了 listen()，客户端就可以通过 connect() 和服务器建立连接，

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	//为一个连接请求提供服务。
	//addrClient包含了发出连接请求的客户机IP地址信息；返回的新socket描述服务器与该客户机的连接
	SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
	while (1)
	{
		char sendBuf[50];//存放要发送的数据;
						 /*
						 inet_ntoa网络地址转换转点分十进制的字符串指针
						 s%是字符串，这里代表inet_ntoa转换的网络地址；
						 sprintf函数就是用将 “欢迎 %s 访问服务端!” 存放在sendBuf数组里;*/
		sprintf(sendBuf, "欢迎 %s 访问服务端!", inet_ntoa(addrClient.sin_addr));


		/*发送数据端:
		sockfd :  指定发送端套接字描述符;
		buff   :  存放要发送数据的缓冲区;
		nbytes :  实际要改善的数据的字节数;
		flags  :  一般置为0;
		send(int sockfd, const void *buff, size_t nbytes, int flags);*/
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);

		/*接收端发送来的数据:
		sockfd  :  接收端套接字描述符;
		buff    :  用来存放recv函数接收到的数据的缓冲区;
		nbytes  :  指明buff的长度;
		flags   :  一般置为0;
		recv(int sockfd, void *buff, size_t nbytes, int flags);*/
		char recvBuf[50];//存放接收的数据;
		recv(sockConn, recvBuf, 50, 0);

		//将接收到的信息显示出来，一般服务器是不需要将接收的内容显示的;
		cout << recvBuf << endl;// printf("%s\n", recvBuf);

		Sleep(2000);//暂停2000毫秒，2秒;
	}
	WSACleanup();
	/*WSAStartup的功能是初始化Winsock DLL，
	WSACleanup是来解除与Socket库的绑定并且释放Socket库所占用的系统资源。*/
}
