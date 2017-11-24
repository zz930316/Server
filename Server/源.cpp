// TcpServer.cpp : �������̨Ӧ�ó������ڵ㡣  

#include <winsock2.h>  
#include <iostream>  

#pragma comment(lib,"ws2_32.lib")  
using namespace std;

#define BUF_SIZE 64  


/*	
			����cmd  ��
netstat -nao        :			�鿴����Щ�˿��ڱ�ʹ��;
netstat -nao | findstr 63790  :  ����63790����˿�;
*/

int main()
{
	WSADATA wsd;
	SOCKET sServer;
	int retVal;
	char buf[BUF_SIZE];

	//��ʼ��Socket  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)//WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//�������ڼ�����Socket  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	addrServ.sin_port = htons(63790);/*htons�����������˿�ת��Ϊ�����ֽ�˳��(to network short);
							htons ���޷��Ŷ�����ת��Ϊ�����ֽ�˳��;*/
	addrServ.sin_addr.S_un.S_addr = htonl(INADDR_ANY);/*htonl������������ַת��Ϊ�����ֽ�˳��(to network long);
					htonl ���޷��ų�����ת��Ϊ�����ֽ�˳��;
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
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	SOCKET sClient;
	//ѭ���ȴ�  

	while (true)
	{
		//Ϊһ�����������ṩ����
		//addrClient�����˷�����������Ŀͻ���IP��ַ��Ϣ�����ص���socket������������ÿͻ���������
		sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
		if (INVALID_SOCKET == sClient)//INVALID_SOCKET ����Ч�׽���
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
		break;

	}
	//ѭ�����ܿͻ��˵����ݣ�ֱ���ͻ��˷���quit������˳�  
	while (true)
	{
		/*void ZeroMemory( PVOID Destination,SIZE_T Length );
			Destination :ָ��һ��׼����0�������ڴ�����Ŀ�ʼ��ַ��
			Length :׼����0�������ڴ�����Ĵ�С�����ֽ������㡣
		ZeroMemoryֻ�ǽ�ָ�����ڴ�����㡣ʹ�ýṹǰ���㣬�����ýṹ�ĳ�Ա��ֵ���в�ȷ���ԣ�
			���ǽ�buf�����е�64��Ԫ��ȫ����ֵΪ0*/
		ZeroMemory(buf, BUF_SIZE);

		/*���ն˷�����������:
			sockfd  :  ���ն��׽���������;
			buff    :  �������recv�������յ������ݵĻ�����;
			nbytes  :  ָ��buff�ĳ���;
			flags   :  һ����Ϊ0;
			recv(int sockfd, void *buff, size_t nbytes, int flags);*/
		retVal = recv(sClient, buf, BUFSIZ, 0);
		if (SOCKET_ERROR == retVal)		//SOCKET_ERROR 0 �ӿڴ���
		{
			int err = WSAGetLastError();	//ָ�ú��������ϴη������������,��������кܶ��ַ���ֵ ���ٶ�...
			if (err == WSAEWOULDBLOCK)		//WSAEWOULDBLOCK : 10035 ��Դ��ʱ������
			{
				Sleep(100);
				continue;
			}
			else if (err == WSAETIMEDOUT || err == WSAENETDOWN)
			{	//WSAETIMEDOUT��10060 - ���ӳ�ʱ    WSAENETDOWN��10050 - ����Ͽ�
				
				cout << "recv failed!" << endl;
				closesocket(sServer);	//�ر���sServer����������IP���˿ڵ�����
				closesocket(sClient);	//�ر���sClient���ͻ��ˣ�IP���˿ڵ�����
				WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
				return -1;
			}
		}
		//��ȡϵͳʱ��  
		SYSTEMTIME st;			//����ṹ�����꣬�£��գ����ڣ�ʱ���֣��룬�����;
		GetLocalTime(&st);		//��ȡ���صĵ�ǰϵͳ���ں�ʱ��,�����st�ṹ����;
		char sDateTime[30];

		//��SYSTEMTIME�ṹ���е�ʱ����Ϣ�����sDateTime������
		sprintf_s(sDateTime, "%4d-%2d-%2d %2d:%2d:%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		//��ӡ�����Ϣ  
		printf("%s,Recv From Client [%s:%d]:%s\n", 
			sDateTime, inet_ntoa(addrClient.sin_addr), addrClient.sin_port, buf);
			/*inet_ntoa�����ַת��ת���ʮ���Ƶ��ַ���ָ��;*/
			
		//����ͻ��˷��͡�quit���ַ�������������˳�  
		if (strcmp(buf, "quit") == 0)
		{	
	/*�������ݶ�:	sockfd :  ָ�����Ͷ��׽���������;
					buff   :  ���Ҫ�������ݵĻ�����;
					nbytes :  ʵ��Ҫ���Ƶ����ݵ��ֽ���;
					flags  :  һ����Ϊ0;
				send(int sockfd, const void *buff, size_t nbytes, int flags);
				���޴�������send()�������������ݵ���������ע��������ֿ���С��len�����涨�Ĵ�С����
				����Ļ�������//SOCKET_ERROR 0 �ӿڴ���Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣*/
			retVal = send(sClient, "quit", strlen("quit"), 0);
			break;
		}
		else
		{
			char msg[BUF_SIZE];		//�Զ����    BUF_SIZE  64
			
			//��SYSTEMTIME�ṹ���е�ʱ����Ϣ�����sDateTime������
			sprintf_s(msg, "Message received - %s", buf);
			while (true)
			{
				/*�������ݶ�:
					sockfd :  ָ�����Ͷ��׽���������;
					buff   :  ���Ҫ�������ݵĻ�����;
					nbytes :  ʵ��Ҫ���Ƶ����ݵ��ֽ���;
					flags  :  һ����Ϊ0;
				send(int sockfd, const void *buff, size_t nbytes, int flags);
				���޴�������send()�������������ݵ���������ע��������ֿ���С��len�����涨�Ĵ�С����
				����Ļ�������//SOCKET_ERROR 0 �ӿڴ���Ӧ�ó����ͨ��WSAGetLastError()��ȡ��Ӧ������롣*/
				retVal = send(sClient, msg, strlen(msg), 0);
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
						closesocket(sClient);	//�ر�sClient���ͻ��ˣ�IP���˿�����
						WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
						return -1;
					}
				}
				break;
			}
		}
	}

	//�ͷ�Socket  
	closesocket(sServer);	//�ر�sServer����������IP���˿�����
	closesocket(sClient);	//�ر�sClient���ͻ��ˣ�IP���˿�����
	WSACleanup();/*WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/

	system("pause");
	return 0;
}