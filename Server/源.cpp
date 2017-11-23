#include <iostream>  
#include <Winsock2.h> //windows socket��ͷ�ļ�  

#pragma comment( lib, "ws2_32.lib" )// ����Winsock2.h�ľ�̬���ļ�  
using namespace std;

void main()
{
	//��ʼ��winsocket  
	WORD wVersionRequested;
	WSADATA wsaData;

	//��winsock DLL����̬���ӿ��ļ������г�ʼ����Э��Winsock�İ汾֧�֣��������Ҫ����Դ��
	WSAStartup(MAKEWORD(1, 1), &wsaData);

	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);	//����socket��AF_INET��ʾ��Internet��ͨ�ţ�
														//SOCK_STREAM��ʾsocket�����׽��֣���Ӧtcp��
														//0ָ������Э��ΪTCP/IP  
	SOCKADDR_IN addrSrv;

						/*htonl������������ַת��Ϊ�����ֽ�˳��(to network long);
					htonl ���޷��ų�����ת��Ϊ�����ֽ�˳��;*/
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY����ָ����ַΪ0.0.0.0�ĵ�ַ,��ʾ��ȷ����ַ,�������ַ������  
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4000);/*htons�����������˿�ת��Ϊ�����ֽ�˳��(to network short);
							htons ���޷��Ŷ�����ת��Ϊ�����ֽ�˳��;*/

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));//�����ص�ַ�󶨵���������socket�ϣ���ʹ�������ϱ�ʶ��socket

	listen(sockSrv, 5);//socket������׼�����ܿͻ���ͨ�� connect()��������������  
					   //listen()������������������Ҫ��������Ϊ�������׽��ֺ��׽��ֶ�Ӧ�����Ӷ��г��ȸ��� Linux �ںˣ�Ȼ��listen()�����ͽ�����
					   //ֻҪ TCP ������������ listen()���ͻ��˾Ϳ���ͨ�� connect() �ͷ������������ӣ�

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	//Ϊһ�����������ṩ����
	//addrClient�����˷�����������Ŀͻ���IP��ַ��Ϣ�����ص���socket������������ÿͻ���������
	SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
	while (1)
	{
		char sendBuf[50];//���Ҫ���͵�����;
						 /*
						 inet_ntoa�����ַת��ת���ʮ���Ƶ��ַ���ָ��
						 s%���ַ������������inet_ntoaת���������ַ��
						 sprintf���������ý� ����ӭ %s ���ʷ����!�� �����sendBuf������;*/
		sprintf(sendBuf, "��ӭ %s ���ʷ����!", inet_ntoa(addrClient.sin_addr));


		/*�������ݶ�:
		sockfd :  ָ�����Ͷ��׽���������;
		buff   :  ���Ҫ�������ݵĻ�����;
		nbytes :  ʵ��Ҫ���Ƶ����ݵ��ֽ���;
		flags  :  һ����Ϊ0;
		send(int sockfd, const void *buff, size_t nbytes, int flags);*/
		send(sockConn, sendBuf, strlen(sendBuf) + 1, 0);

		/*���ն˷�����������:
		sockfd  :  ���ն��׽���������;
		buff    :  �������recv�������յ������ݵĻ�����;
		nbytes  :  ָ��buff�ĳ���;
		flags   :  һ����Ϊ0;
		recv(int sockfd, void *buff, size_t nbytes, int flags);*/
		char recvBuf[50];//��Ž��յ�����;
		recv(sockConn, recvBuf, 50, 0);

		//�����յ�����Ϣ��ʾ������һ��������ǲ���Ҫ�����յ�������ʾ��;
		cout << recvBuf << endl;// printf("%s\n", recvBuf);

		Sleep(2000);//��ͣ2000���룬2��;
	}
	WSACleanup();
	/*WSAStartup�Ĺ����ǳ�ʼ��Winsock DLL��
	WSACleanup���������Socket��İ󶨲����ͷ�Socket����ռ�õ�ϵͳ��Դ��*/
}