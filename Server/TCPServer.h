#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <winsock2.h>  

#pragma comment(lib,"ws2_32.lib")  
using namespace std;

/* ��ſͻ�����Ϣ */
struct ClientInfo
{
	SOCKET fSocket; //socket
	SOCKADDR_IN fAddress; //�ͻ���ip��˿�
	string fArriveTime; //�ͻ�������ʱ��
};

/* TCPServer����� */
class TCPServer
{
public:
	enum  ABC { kBlocking, kNonBlocking }; //��������
	enum { kMaxSendBufSize = 96u * 1024u, kMaxRecvBufSize = 96u * 1024u };
	enum { kMaxTCPPacketSize = 5 };//�����Ϣ�ڴ��С
	TCPServer();
	~TCPServer();
	bool SetIOType(int inIOType);	//��������
	bool Open();//��
	bool Close();	//�ر�
	int Listen(const unsigned short& inPort);	//����	
	int Send(ClientInfo  inClientInfo, const char* inData, const size_t& inSize);//������Ϣ
	int Recv(ClientInfo  inClientInfo, char* outBuffer, const size_t inBufferSize);		//������Ϣ
	ClientInfo  Accept();//���տͻ�������
private:
	ClientInfo client;
	queue<ClientInfo > fClientQueue; //����������ӵĿͻ���, ���Ƚ��ȳ��ķ�ʽ����
	map<SOCKET, ClientInfo > fClientMap; //���socket�׽��ֶ�Ӧ�Ŀͻ�����Ϣ
	int retVal;
	SOCKET fSeversocket;//���������׽���
};