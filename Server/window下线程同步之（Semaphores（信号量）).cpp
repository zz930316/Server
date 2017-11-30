#include <iostream> 
#include <windows.h> 
using namespace std;

const int g_Number = 3;
DWORD WINAPI ThreadProc1(__in  LPVOID lpParameter);
DWORD WINAPI ThreadProc2(__in  LPVOID lpParameter);
DWORD WINAPI ThreadProc3(__in  LPVOID lpParameter);

HANDLE hSemp1, hSemp2, hSemp3;	// HANDLE   ����������ַ����˼

void SetInt(int *val)
{
	*val = 2;
}

int main()
{
	int a;
	SetInt(&a);
	// ����һ���µ��ź���;��ִ�гɹ��������ź�������ľ�������ʾ����
	hSemp1 = CreateSemaphore(NULL, 1, 1, NULL);
	hSemp2 = CreateSemaphore(NULL, 0, 1, NULL);
	hSemp3 = CreateSemaphore(NULL, 0, 1, NULL);

	HANDLE hThread[g_Number] = { 0 };
	int first = 1, second = 2, third = 3;
	hThread[0] = CreateThread(NULL, 0, ThreadProc1, (LPVOID)first, 0, NULL); //�������ܣ������߳�
	hThread[1] = CreateThread(NULL, 0, ThreadProc2, (LPVOID)second, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ThreadProc3, (LPVOID)third, 0, NULL);

	WaitForMultipleObjects(g_Number, hThread, TRUE, INFINITE);//�ȴ�g_Number���̣߳�hThread��������INFINITE  ���޵ȴ���ʱ�䣩
	CloseHandle(hThread[0]); //�ر�һ���ں˶������а����ļ����ļ�ӳ�䡢���̡��̡߳���ȫ��ͬ�������
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);//�ر��߳�

	CloseHandle(hSemp1);//�ر��ź���
	CloseHandle(hSemp2);
	CloseHandle(hSemp3);
	return 0;
}

DWORD WINAPI ThreadProc1(__in  LPVOID lpParameter)
{
	WaitForSingleObject(hSemp1, INFINITE);//�ȴ��ź��� ��INFINITE  ���޵ȴ���ʱ�䣩	P(S)��һ
	cout << (int)lpParameter << endl;
	ReleaseSemaphore(hSemp2, 1, NULL);//�ͷ��ź�����ռ��Ȩ	V(S)  ��һ
	return 0;
}

DWORD WINAPI ThreadProc2(__in  LPVOID lpParameter)
{
	WaitForSingleObject(hSemp2, INFINITE);//�ȴ��ź��� 
	cout << (int)lpParameter << endl;
	ReleaseSemaphore(hSemp3, 1, NULL);//�ͷ��ź�����ռ��Ȩ ReleaseSemaphore�������ڶ�ָ�����ź�������ָ����ֵ��
	return 0;
}

DWORD WINAPI ThreadProc3(__in  LPVOID lpParameter)
{
	WaitForSingleObject(hSemp3, INFINITE);//�ȴ��ź��� 
	cout << (int)lpParameter << endl;
	return 0;
}