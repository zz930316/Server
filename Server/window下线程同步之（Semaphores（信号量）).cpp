#include <iostream> 
#include <windows.h> 
using namespace std;

const int g_Number = 3;
DWORD WINAPI ThreadProc1(__in  LPVOID lpParameter);
DWORD WINAPI ThreadProc2(__in  LPVOID lpParameter);
DWORD WINAPI ThreadProc3(__in  LPVOID lpParameter);

HANDLE hSemp1, hSemp2, hSemp3;	// HANDLE   句柄，保存地址的意思

void SetInt(int *val)
{
	*val = 2;
}

int main()
{
	int a;
	SetInt(&a);
	// 创建一个新的信号量;如执行成功，返回信号量对象的句柄；零表示出错
	hSemp1 = CreateSemaphore(NULL, 1, 1, NULL);
	hSemp2 = CreateSemaphore(NULL, 0, 1, NULL);
	hSemp3 = CreateSemaphore(NULL, 0, 1, NULL);

	HANDLE hThread[g_Number] = { 0 };
	int first = 1, second = 2, third = 3;
	hThread[0] = CreateThread(NULL, 0, ThreadProc1, (LPVOID)first, 0, NULL); //函数功能：创建线程
	hThread[1] = CreateThread(NULL, 0, ThreadProc2, (LPVOID)second, 0, NULL);
	hThread[2] = CreateThread(NULL, 0, ThreadProc3, (LPVOID)third, 0, NULL);

	WaitForMultipleObjects(g_Number, hThread, TRUE, INFINITE);//等待g_Number个线程（hThread）结束；INFINITE  无限等待（时间）
	CloseHandle(hThread[0]); //关闭一个内核对象。其中包括文件、文件映射、进程、线程、安全和同步对象等
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);//关闭线程

	CloseHandle(hSemp1);//关闭信号量
	CloseHandle(hSemp2);
	CloseHandle(hSemp3);
	return 0;
}

DWORD WINAPI ThreadProc1(__in  LPVOID lpParameter)
{
	WaitForSingleObject(hSemp1, INFINITE);//等待信号量 ；INFINITE  无限等待（时间）	P(S)减一
	cout << (int)lpParameter << endl;
	ReleaseSemaphore(hSemp2, 1, NULL);//释放信号量的占有权	V(S)  加一
	return 0;
}

DWORD WINAPI ThreadProc2(__in  LPVOID lpParameter)
{
	WaitForSingleObject(hSemp2, INFINITE);//等待信号量 
	cout << (int)lpParameter << endl;
	ReleaseSemaphore(hSemp3, 1, NULL);//释放信号量的占有权 ReleaseSemaphore函数用于对指定的信号量增加指定的值。
	return 0;
}

DWORD WINAPI ThreadProc3(__in  LPVOID lpParameter)
{
	WaitForSingleObject(hSemp3, INFINITE);//等待信号量 
	cout << (int)lpParameter << endl;
	return 0;
}