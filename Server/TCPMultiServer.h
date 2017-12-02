#pragma once
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <winsock2.h>  

#pragma comment(lib,"ws2_32.lib")  
using namespace std;

DWORD WINAPI  ThreadFun_accept(LPVOID pM);//���տͻ�������
DWORD WINAPI  ThreadFun_recv(LPVOID pM);



/* ��ſͻ�����Ϣ */
struct ClientInfo
{
	SOCKET fSocket; //socket
	SOCKADDR_IN fAddress; //�ͻ���ip��˿�
	string fArriveTime; //�ͻ�������ʱ��
	
};

/* TCPServer����� */
class TCPMultiServer
{
public:
	enum  ABC { kBlocking, kNonBlocking }; //��������
	enum { kMaxSendBufSize = 96u * 1024u, kMaxRecvBufSize = 96u * 1024u };
	enum { kMaxTCPPacketSize = 5 };//�����Ϣ�ڴ��С

	TCPMultiServer();
	~TCPMultiServer();

	bool SetIOType(int inIOType);	//��������
	bool Open();	//��
	bool Close();	//�ر�
	int Listen(const unsigned short& inPort);	//����	
	void Create_Thread();//�����߳�
	int Send();//������Ϣ
	int Recv();		//������Ϣ
	int Accept();//���տͻ�������	
	int retVal_r = 0;
	bool judge_j = false;
	
private:
	ClientInfo fClient,fClient_two;
	SOCKET fclient;
	queue<ClientInfo > fClientQueue; //����������ӵĿͻ���, ���Ƚ��ȳ��ķ�ʽ����
	map<SOCKET, ClientInfo > fClientMap; //���socket�׽��ֶ�Ӧ�Ŀͻ�����Ϣ
	SOCKET fSeversocket;//���������׽���
	SYSTEMTIME st;			//����ṹ�����꣬�£��գ����ڣ�ʱ���֣��룬�����;
	HANDLE hThread[2];//�����߳���
	
};