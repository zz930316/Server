//��򵥵Ĵ������߳�ʵ��  
#include <stdio.h>  
#include <windows.h>  
//���̺߳���  
DWORD WINAPI ThreadFun(LPVOID pM)
{
	
	printf("���̵߳��߳�ID��Ϊ��%d\n���߳����Hello World\n", GetCurrentThreadId()); /*GetCurrentThreadId()��ȡ��ǰ�߳�һ��Ψһ���̱߳�ʶ��*/
	return 0;
}
//����������ν��������ʵ�������߳�ִ�еĺ�����  
int main()
{
	printf("     ��򵥵Ĵ������߳�ʵ��\n");
	HANDLE handle = CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);
	/*CreateThread()   �������ܣ������߳�
		��һ��������ʾ�߳��ں˶���İ�ȫ���ԣ�һ�㴫��NULL��ʾʹ��Ĭ�����á�
		�ڶ���������ʾ�߳�ջ�ռ��С������0��ʾʹ��Ĭ�ϴ�С��1MB����
		������������ʾ���߳���ִ�е��̺߳�����ַ������߳̿���ʹ��ͬһ��������ַ��
		���ĸ������Ǵ����̺߳����Ĳ�����
		���������ָ������ı�־�������̵߳Ĵ�����Ϊ0��ʾ�̴߳���֮�������Ϳ��Խ��е��ȣ����ΪCREATE_SUSPENDED���ʾ�̴߳�������ͣ���У����������޷����ȣ�ֱ������ResumeThread()��
		�����������������̵߳�ID�ţ�����NULL��ʾ����Ҫ���ظ��߳�ID�š�
		��������ֵ��
		�ɹ��������̵߳ľ����ʧ�ܷ���NULL��*/

	WaitForSingleObject(handle, INFINITE);
	/*�ڶ��� WaitForSingleObject
		�������ܣ��ȴ����� �C ʹ�߳̽���ȴ�״̬��ֱ��ָ�����ں˶��󱻴�����
	����˵����
		��һ������ΪҪ�ȴ����ں˶���
		�ڶ�������Ϊ��ȴ���ʱ�䣬�Ժ���Ϊ��λ���紫��5000�ͱ�ʾ5�룬����0���������أ�����INFINITE��ʾ���޵ȴ���
		��Ϊ�̵߳ľ�����߳�����ʱ��δ�����ģ��߳̽������У�������ڴ���״̬�����Կ�����WaitForSingleObject()���ȴ�һ���߳̽������С�
		��������ֵ��
		��ָ����ʱ���ڶ��󱻴�������������WAIT_OBJECT_0��������ȴ�ʱ�������δ����������WAIT_TIMEOUT����������д��󽫷���WAIT_FAILED*/
	return 0;
}


//�������Ӹ��߳�ʵ��  
#include <stdio.h>  
#include <process.h>  
#include <windows.h>  
#include <iostream>
using namespace std;
//���̺߳���  
unsigned int __stdcall ThreadFun(PVOID pM)
{
	printf("�߳�ID��Ϊ%4d�����߳�˵��Hello World\n", GetCurrentThreadId());
	return 0;
}
//����������ν��������ʵ�������߳�ִ�еĺ�����  
int main()
{
	printf("     ����������߳�ʵ�� \n");
	const int THREAD_NUM = 5;
	HANDLE handle[THREAD_NUM];
	for (int i = 0; i < THREAD_NUM; i++)
	{
		Sleep(10);
		handle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun,NULL, 0, NULL);
	}

	WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);//���߳̿��Ƶȴ�
// 	for (i = 0; i < THREAD_NUM; i++)
// 		WaitForSingleObject(handle[i], INFINITE);				//���߳̿��Ƶȴ�
	return 0;
}


#include <iostream>
using namespace std;
typedef void(*p)(int*);

void printInt(int *val)
{
	cout << /*(int)*/*val << endl;
}

void abc(p prin, int* aaa)
{
	prin((int*)aaa);
}
int main()
{
	int a = 333;
	abc(printInt, &a);
	return 0;
}