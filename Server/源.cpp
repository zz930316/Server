

//   ��ѭ����Ƶ���Ĵ��������һ��ʱ��ͣ�ٿ�����cpuʹ���ʽ���

  // ���̷߳�����������
/*

htons = Host To Network Short
ntohs = Network To Host Short

*/

/*	
			����cmd  ��
netstat -nao        :			�鿴����Щ�˿��ڱ�ʹ��;
netstat -nao | findstr 63790  :  ����63790����˿�;
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


SOCKET sServer;//�������׽���
SOCKET sClient_1, sClient_2;//�ͻ����׽���
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
	//��ʼ��Socket  
	//MAKEWORD(1, 1)��MAKEWORD(2, 2)���������ڣ�ǰ��ֻ��һ�ν���һ�Σ��������Ϸ��ͣ���������
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//�������ڼ�����Socket  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		/*����socket��AF_INET��ʾ��Internet��ͨ�ţ�
	   sock_stream   ���б��ϵģ����ܱ�֤������ȷ���͵��Է����������ӵ�SOCKET;
	   IPPROTO_TCP �� IPPROTO_IP�������ֲ�ͬ��Э��,�ֱ����IPЭ���������TCPЭ���IPЭ��
	   Ҳ���Խ�IPPROTO_TCP����0, 0 ��������Э�� TCP ���� IP  */
	if (INVALID_SOCKET == sServer)//INVALID_SOCKET  0  ����Ч�׽���
	{
		cout << "socket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	//����SocketΪ������ģʽ  
	int iMode = 1;
	/*int ioctlsocket( SOCKET s, long cmd, u_long FAR *argp );
	s��һ����ʶ�׽ӿڵ�������;		cmd�����׽ӿ�s�Ĳ�������;		argp��ָ��cmd��������������ָ��;*/
	retVal = ioctlsocket(sServer, FIONBIO, (u_long FAR*)&iMode); //FIONBIO����socket����Ϊ������
																//u_long �޷��ų����� 4���ֽ�
	//��ioctlsocket�����ɹ���ioctlsocket()����0������Ļ�������SOCKET_ERROR����
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "ioctlsocket failed!" << endl;
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	//���÷�����Socket��ַ  
	sockaddr_in addrServ;//�����洢��ַ�Ͷ˿ڵĽṹ��

	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(9513);/*htons�����������˿�ת��Ϊ�����ֽ�˳��(to network short);
							htons ��  �޷��Ŷ�����  ת��Ϊ�����ֽ�˳��;
							htons = Host To Network Short
							htonl = Host To Network long
							ntohs = Network To Host Short
							*/
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/*htonl������������ַת��Ϊ�����ֽ�˳��(to network long);
					htonl ��  �޷��ų�����  ת��Ϊ�����ֽ�˳��;
					htonl = Host To Network long
					htons = Host To Network Short
					ntohs = Network To Host Short
					INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ,��ʾ��ȷ����ַ,�������ַ������  */

	//��Socket //�����ص�ַ�󶨵���������Server�� 
	//����ֵ��ʾ�󶨲����Ƿ�ɹ���0��ʾ�ɹ��� -1��ʾ���ɹ�
	retVal = bind(sServer, (const struct sockaddr*)&addrServ, sizeof(sockaddr_in));

	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);//�ر�sServer����������IP���˿�����
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	//����  û�д�������listen()����0������Ļ�������-1
	retVal = listen(sServer, 1);
	if (retVal == SOCKET_ERROR)//SOCKET_ERROR -1
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);//�ر�sServer����
		WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
		return -1;
	}
	//���ܿͻ�����  
	cout << "TCP��������������" << endl;

	//�����߳�
	HANDLE hThread[2] = { 0 };
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun_accept, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun_recv, NULL, 0, NULL);
	//ѭ�����ܿͻ��˵����ݣ�ֱ���ͻ��˷���quit������˳�  
	

	WaitForMultipleObjects(2, hThread, TRUE, INFINITE);//���߳̿��Ƶȴ�
	CloseHandle(hThread[0]); //�ر�һ���ں˶������а����ļ����ļ�ӳ�䡢���̡��̡߳���ȫ��ͬ�������
	CloseHandle(hThread[1]);
	 //�ͷ�Socket  
	closesocket(sServer);	//�ر�sServer����������IP���˿�����
	map_sock::iterator map_it = ClientMap.begin();
	while (true)
	{
		if (map_it != ClientMap.end())
			break;
		closesocket(map_it->first);	//�ر�sClient���ͻ��ˣ�IP���˿�����
		map_it++;
	}
	WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/

	system("pause");
	return 0;
}




//���̺߳���  
unsigned int __stdcall ThreadFun_accept(PVOID pM)
{
	while (true)
	{
		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);
		sClient_1 = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == sClient_1)//INVALID_SOCKET ����Ч�׽���
		{
			int err = WSAGetLastError(); //ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
			if (err == WSAEWOULDBLOCK)//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
			{
				Sleep(100);
				continue;
			}

			else
			{
				cout << "accept failed!" << endl;
				closesocket(sServer);//�ر�sServer����������IP���˿�����
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				return -1;
			}
		}
		ClientMap.insert(map_sock::value_type(sClient_1, addrClient));
		//��ȡϵͳʱ��  
		SYSTEMTIME st;			//����ṹ�����꣬�£��գ����ڣ�ʱ���֣��룬�����;
		GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
		char sDateTime[30];
		//��SYSTEMTIME�ṹ���е�ʱ����Ϣ�����sDateTime������
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		cout << sDateTime << "�пͻ���IP:" << inet_ntoa(ClientMap[sClient_1].sin_addr) <<
			"  �˿ڣ�" << ntohs(ClientMap[sClient_1].sin_port) << " ���ӷ������� " << endl;
/*		ntohs ��  �޷��Ŷ�����  ת��Ϊ�����ֽ�˳��;
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
					if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 �ӿڴ���
					{
						int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
						if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
						{
							Sleep(100);
							map_it++;
							continue;
						}
						else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
						{	//WSAETIMEDOUT��10060 - ���ӳ�ʱ    WSAENETDOWN��10050 - ����Ͽ�

							cout << "recv failed!" << endl;
							closesocket(sServer);	//�ر���sServer����������IP���˿ڵ�����
							closesocket(sClient_2);	//�ر���sClient���ͻ��ˣ�IP���˿ڵ�����
							WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
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
			//��ȡϵͳʱ��  
			SYSTEMTIME st;			//����ṹ�����꣬�£��գ����ڣ�ʱ���֣��룬�����;
			GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
			char sDateTime[30];
			sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			cout << sDateTime << "���յ��ӿͻ���IP:" << inet_ntoa(ClientMap[sClient_2].sin_addr) <<
				"  �˿ڣ�" << ntohs(ClientMap[sClient_2].sin_port) << "  ������ : " << buf << endl;
			//����ͻ��˷��͡�quit���ַ�������������˳�  
 			if (strcmp(buf, "quit") == 0)
 			{
 				retVal = send(sClient_2, "quit", strlen("quit"), 0);
 				break;
 			}
 			else
 			{
 				char msg[BUF_SIZE];		//�Զ����    BUF_SIZE  64
 										//��SYSTEMTIME�ṹ���е�ʱ����Ϣ�����sDateTime������
 				sprintf_s(msg, "Message received - %s", buf);
 				while (true)
 				{
 					retVal = send(sClient_2, msg, strlen(msg), 0);
 					if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 �ӿڴ���
 					{
 						int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
 						if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
 						{
 							Sleep(100);
 							continue;
 						}
 						else
 						{
 							cout << "send failed!" << endl;
 							closesocket(sServer);	//�ر�sServer����������IP���˿�����
 							closesocket(sClient_2);	//�ر�sClient���ͻ��ˣ�IP���˿�����
 							WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
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