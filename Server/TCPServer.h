#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <winsock2.h>  

#pragma comment(lib,"ws2_32.lib")  
using namespace std;

/* 存放客户端信息 */
struct ClientInfo
{
	SOCKET fSocket; //socket
	SOCKADDR_IN fAddress; //客户端ip与端口
	string fArriveTime; //客户端连接时间
};

/* TCPServer服务端 */
class TCPServer
{
public:
	enum  ABC { kBlocking, kNonBlocking }; //设置阻塞
	enum { kMaxSendBufSize = 96u * 1024u, kMaxRecvBufSize = 96u * 1024u };
	enum { kMaxTCPPacketSize = 5 };//存放信息内存大小
	TCPServer();
	~TCPServer();
	bool SetIOType(int inIOType);	//设置阻塞
	bool Open();//打开
	bool Close();	//关闭
	int Listen(const unsigned short& inPort);	//监听	
	int Send(ClientInfo  inClientInfo, const char* inData, const size_t& inSize);//发送信息
	int Recv(ClientInfo  inClientInfo, char* outBuffer, const size_t inBufferSize);		//接受信息
	ClientInfo  Accept();//接收客户端连接
private:
	ClientInfo client;
	queue<ClientInfo > fClientQueue; //存放所有连接的客户端, 按先进先出的方式排列
	map<SOCKET, ClientInfo > fClientMap; //存放socket套接字对应的客户端信息
	int retVal;
	SOCKET fSeversocket;//服务器的套接字
};