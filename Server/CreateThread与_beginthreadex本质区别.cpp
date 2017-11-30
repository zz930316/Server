//最简单的创建多线程实例  
#include <stdio.h>  
#include <windows.h>  
//子线程函数  
DWORD WINAPI ThreadFun(LPVOID pM)
{
	
	printf("子线程的线程ID号为：%d\n子线程输出Hello World\n", GetCurrentThreadId()); /*GetCurrentThreadId()获取当前线程一个唯一的线程标识符*/
	return 0;
}
//主函数，所谓主函数其实就是主线程执行的函数。  
int main()
{
	printf("     最简单的创建多线程实例\n");
	HANDLE handle = CreateThread(NULL, 0, ThreadFun, NULL, 0, NULL);
	/*CreateThread()   函数功能：创建线程
		第一个参数表示线程内核对象的安全属性，一般传入NULL表示使用默认设置。
		第二个参数表示线程栈空间大小。传入0表示使用默认大小（1MB）。
		第三个参数表示新线程所执行的线程函数地址，多个线程可以使用同一个函数地址。
		第四个参数是传给线程函数的参数。
		第五个参数指定额外的标志来控制线程的创建，为0表示线程创建之后立即就可以进行调度，如果为CREATE_SUSPENDED则表示线程创建后暂停运行，这样它就无法调度，直到调用ResumeThread()。
		第六个参数将返回线程的ID号，传入NULL表示不需要返回该线程ID号。
		函数返回值：
		成功返回新线程的句柄，失败返回NULL。*/

	WaitForSingleObject(handle, INFINITE);
	/*第二个 WaitForSingleObject
		函数功能：等待函数 – 使线程进入等待状态，直到指定的内核对象被触发。
	函数说明：
		第一个参数为要等待的内核对象。
		第二个参数为最长等待的时间，以毫秒为单位，如传入5000就表示5秒，传入0就立即返回，传入INFINITE表示无限等待。
		因为线程的句柄在线程运行时是未触发的，线程结束运行，句柄处于触发状态。所以可以用WaitForSingleObject()来等待一个线程结束运行。
		函数返回值：
		在指定的时间内对象被触发，函数返回WAIT_OBJECT_0。超过最长等待时间对象仍未被触发返回WAIT_TIMEOUT。传入参数有错误将返回WAIT_FAILED*/
	return 0;
}


//创建多子个线程实例  
#include <stdio.h>  
#include <process.h>  
#include <windows.h>  
#include <iostream>
using namespace std;
//子线程函数  
unsigned int __stdcall ThreadFun(PVOID pM)
{
	printf("线程ID号为%4d的子线程说：Hello World\n", GetCurrentThreadId());
	return 0;
}
//主函数，所谓主函数其实就是主线程执行的函数。  
int main()
{
	printf("     创建多个子线程实例 \n");
	const int THREAD_NUM = 5;
	HANDLE handle[THREAD_NUM];
	for (int i = 0; i < THREAD_NUM; i++)
	{
		Sleep(10);
		handle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun,NULL, 0, NULL);
	}

	WaitForMultipleObjects(THREAD_NUM, handle, TRUE, INFINITE);//多线程控制等待
// 	for (i = 0; i < THREAD_NUM; i++)
// 		WaitForSingleObject(handle[i], INFINITE);				//单线程控制等待
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