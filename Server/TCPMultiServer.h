#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <winsock2.h>  

#pragma comment(lib,"ws2_32.lib")  
using namespace std;

DWORD WINAPI  ThreadFun_accept(LPVOID pM);//接收客户端连接
DWORD WINAPI  ThreadFun_recv(LPVOID pM);



/* 存放客户端信息 */
struct ClientInfo
{
	SOCKET fSocket; //socket
	SOCKADDR_IN fAddress; //客户端ip与端口
	string fArriveTime; //客户端连接时间
	
};

/* TCPServer服务端 */
class TCPMultiServer
{
public:
	enum  ABC { kBlocking, kNonBlocking }; //设置阻塞
	enum { kMaxSendBufSize = 96u * 1024u, kMaxRecvBufSize = 96u * 1024u };
	enum { kMaxTCPPacketSize = 5 };//存放信息内存大小

	TCPMultiServer();
	~TCPMultiServer();

	bool SetIOType(int inIOType);	//设置阻塞
	bool Open();	//打开
	bool Close();	//关闭
	int Listen(const unsigned short& inPort);	//监听	
	void Create_Thread();//创建线程
	int Send();//发送信息
	int Recv();		//接受信息
	int Accept();//接收客户端连接	
	int retVal_r = 0;
	bool judge_j = false;
	
private:
	ClientInfo fClient,fClient_two;
	SOCKET fclient;
	queue<ClientInfo > fClientQueue; //存放所有连接的客户端, 按先进先出的方式排列
	map<SOCKET, ClientInfo > fClientMap; //存放socket套接字对应的客户端信息
	SOCKET fSeversocket;//服务器的套接字
	SYSTEMTIME st;			//这个结构体存放年，月，日，星期，时，分，秒，毫秒等;
	HANDLE hThread[2];//创建线程数
	
};