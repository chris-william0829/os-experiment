#include<iostream>
#include<Windows.h>
#include<time.h>
#include<string.h>

using namespace std;

//定义HANDLE对象，用来存储创建的生产者消费者进程句柄
HANDLE handleProcess[7];
char Name[4] = { 'C','H','E','N' };

typedef struct _buffer
{
	char buffer[4];
	int write;
	int read;
}buffer;

void sleepRandomTime()
{
	//随机一个3000ms以内的数，并休眠
	int randomTime = rand() % 3000;
	Sleep(randomTime);
}

char randChar()
{
	//随机一个大写英文字母作为生产者生产的产品
	return Name[rand() % 4];
}

//创建进程对象
void createProcess(int id)
{
	//定义字符数组用来存放文件路径，MAX_PATH为宏定义=260
	//TCHAR为适用于UNICODE和ANSI编码的字符变量，可以自适应调整
	TCHAR filePath[MAX_PATH];
	//获得当前执行程序的路径
	GetModuleFileName(NULL, filePath, MAX_PATH);
	TCHAR cmd[MAX_PATH];
	sprintf_s(cmd, "\"%s\" %d", filePath, id);
	//cout << cmd << endl;
	//定义STARTUPINFO结构变量用于指定新进程子窗口特性，并进行初始化
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	//定义PROCESS_INFORMATION结构变量记录新进程的信息。
	PROCESS_INFORMATION pi;

	/*
	 1).第一个参数为子进程的文件名
	 2).第二个参数为命令行参数
	 3).第三个参数指定为NULL，为默认的进程安全描述符。
	 4).第四个参数指定为NULL，为默认的线程安全描述符。
	 5).第五个参数指定为FALSE，设置子进程不可继承当前进程的句柄。
	 6).第六个参数指定为0，表示子进程不创建新控制台窗口。
	 7).第七个参数指定为NULL，表示子进程使用当前进程的环境。
	 8).第八个参数指定为NULL，表示子进程使用当前进程的的驱动器和目录。
	 9).第九个参数为控制子进程的主窗体如何显示的结构体，此处为上步定义的si。
	 10).第十个参数为&pi，为保存新进程信息的结构体。
	*/
	if (!(CreateProcess(filePath, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)))
	{
		DWORD ErrorCode = GetLastError();
		cout << ErrorCode << endl;
	}
	else
	{
		handleProcess[id] = pi.hProcess;
	}
}

void producer()
{
	//创建互斥信号量
	//HANDLE mutex = CreateSemaphore(NULL, 1, 1, "MYMUTEX");
	//CreateMutex("安全属性","是否被占用","命名")
	HANDLE mutex = CreateMutex(NULL, FALSE, "MUTEX");
	//请求信号量
	HANDLE empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
	HANDLE full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");

	//打开文件映射对象存放在句柄中，缓冲区
	//OpenFileMapping("访问模式" "继承标志" "文件映射对象名指针")
	HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FILEMAP");
	//MapViewOfFile(文件映射对象句柄，访问模式，文件偏移高32位，文件偏移低32位，映射视图大小) 为0映射整个文件
	LPVOID data = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	//强制类型转换
	buffer *bufferData = reinterpret_cast<buffer *>(data);

	//重复4次
	for (int i = 0; i < 4; i++)
	{
		//P操作，申请empty信号量
		WaitForSingleObject(empty, INFINITE);
		//随机休眠3s以内的时间
		sleepRandomTime();

		//P操作，申请互斥信号量，用于互斥只有一个进程访问缓冲区
		WaitForSingleObject(mutex, INFINITE);

		//往缓冲区送入产品
		char product = randChar();
		bufferData->buffer[bufferData->write] = product;
		bufferData->write = (bufferData->write + 1) % 4;

		//释放信号量，必须先释放full，后释放mutex
		ReleaseSemaphore(full, 1, NULL);
		ReleaseMutex(mutex);

		printf("生产者%d向缓冲区写入数据%c\n", (int)GetCurrentProcessId(),product);
		fflush(stdout);
		printf("缓冲区:\t%c\t%c\t%c\t%c\n", bufferData->buffer[0], bufferData->buffer[1], bufferData->buffer[2], bufferData->buffer[3]);
		fflush(stdout);
	}
	//解除文件映射
	UnmapViewOfFile(data);
	data = NULL;
	CloseHandle(mutex);
	CloseHandle(full);
	CloseHandle(empty);
}

void consumer()
{
	//创建互斥信号量
	//HANDLE mutex = CreateSemaphore(NULL, 1, 1, "MYMUTEX");
	//CreateMutex("安全属性","是否被占用","命名")
	HANDLE mutex = CreateMutex(NULL, FALSE, "MUTEX");
	//请求信号量
	HANDLE empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
	HANDLE full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");

	//打开文件映射对象存放在句柄中
	//OpenFileMapping("访问模式" "继承标志" "文件映射对象名指针")
	HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FILEMAP");
	//MapViewOfFile(文件映射对象句柄，访问模式，文件偏移高32位，文件偏移低32位，映射视图大小) 为0映射整个文件
	LPVOID data = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	//强制类型转换
	buffer *bufferData = reinterpret_cast<buffer *>(data);
	for (int i = 0; i < 3; i++)
	{
		//P操作，申请full信号量访问缓冲区
		WaitForSingleObject(full, INFINITE);
		//随机休眠3s以内的时间
		sleepRandomTime();
		//P操作，申请mutex，执行取产品动作
		WaitForSingleObject(mutex, INFINITE);

		char product = bufferData->buffer[bufferData->read];
		bufferData->buffer[bufferData->read] = '-';
		bufferData->read = (bufferData->read + 1) % 4;

		//V操作，同样不能交换顺序
		ReleaseSemaphore(empty,1,NULL);
		ReleaseMutex(mutex);
		
		
		printf("消费者%d向缓冲区取出数据%c\n", (int)GetCurrentProcessId(),product);
		fflush(stdout);
		printf("缓冲区:\t%c\t%c\t%c\t%c\n", bufferData->buffer[0], bufferData->buffer[1], bufferData->buffer[2], bufferData->buffer[3]);
		fflush(stdout);
	}
	//解除文件映射
	UnmapViewOfFile(data);
	data = NULL;
	CloseHandle(mutex);
	CloseHandle(full);
	CloseHandle(empty);
}

int main(int argc, char *argv[])
{
	int processID = 8;
	srand(time(0));
	if (argc > 1)
	{
		sscanf_s(argv[1], "%d", &processID);
	}
	if (processID < 3)
	{
		producer();
	}
	else if (processID < 7)
	{
		consumer();
	}
	else
	{
		//CreateFileMapping("物理文件句柄","安全设置","保护设置","文件映射的最大长度的高32位","文件映射的最大长度的低32位","文件映射对象名")
		//以页式临时文件创建一个文件映射区域，大小位BUFFER的缓冲区FILEMAP
		HANDLE hMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(buffer), "FILEMAP");

		if (hMap != INVALID_HANDLE_VALUE)
		{
			//MapViewOfFile(文件映射对象句柄，访问模式，文件偏移高32位，文件偏移低32位，映射视图大小) 为0映射整个文件
			LPVOID data = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
			if (data != NULL)
			{
				memset(data, 0, sizeof(buffer));
			}
			buffer *bufferData = reinterpret_cast<buffer *>(data);
			memset(bufferData->buffer, '-', sizeof(bufferData->buffer));
			bufferData->read = 0;
			bufferData->write = 0;

			UnmapViewOfFile(data);
			data = NULL;
		}

		//CreateSemaphore("信号量属性","初始值","最大值","命名")
		HANDLE empty = CreateSemaphore(NULL, 4, 4, "EMPTY");
		HANDLE full = CreateSemaphore(NULL, 0, 4, "FULL");
		for (int i = 0; i < 7; i++)
		{
			createProcess(i);
		}
		WaitForMultipleObjects(7, handleProcess, TRUE, INFINITE);
		CloseHandle(empty);
		CloseHandle(full);
	}

}