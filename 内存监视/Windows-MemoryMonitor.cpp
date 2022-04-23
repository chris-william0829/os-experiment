#include<iostream>
#include<Windows.h>
#include<Psapi.h>
#include<TlHelp32.h>
#include<Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
#include<iomanip>

using namespace std;
void ShowSystemMemoryStatus(MEMORYSTATUSEX Statusex)
{

	cout << "-----------------------内存信息-----------------------" << endl;
	//内存使用率
	cout << "物理内存的使用率为: " << Statusex.dwMemoryLoad << "%%" << endl;
	//物理内存
	cout << "物理内存的总容量为: " << ((float)Statusex.ullTotalPhys / 1024 / 1024 / 1024) << "GB." << endl;
	//可用物理内存
	cout << "可用的物理内存为: " << ((float)Statusex.ullAvailPhys / 1024 / 1024 / 1024) << "GB." << endl;
	//提交的内存限制
	cout << "总的交换文件为: " << ((float)Statusex.ullTotalPageFile / 1024 / 1024 / 1024) << "GB." << endl;
	//当前进程可以提交的最大内存量
	cout << "可用的交换文件为：" << ((float)Statusex.ullAvailPageFile / 1024 / 1024 / 1024) << "GB." << endl;
	//虚拟内存
	cout << "虚拟内存的总容量为：" << ((float)Statusex.ullTotalVirtual / 1024 / 1024 / 1024) << "GB." << endl;
	//可用虚拟内存
	cout << "可用的虚拟内存为：" << ((float)Statusex.ullAvailVirtual / 1024 / 1024 / 1024) << "GB." << endl;
	//保留字段
	cout << "保留字段的容量为：" << Statusex.ullAvailExtendedVirtual << "Byte." << endl;
	cout << "------------------------------------------------------" << endl;
}

void ShowSystemInfo(SYSTEM_INFO SysInfo)
{
	cout << "---------------------系统信息-------------------------" << endl;
	cout << "内存页的大小为：" << (int)SysInfo.dwPageSize / 1024 << "KB.";
	cout << "每个进程可用地址空间的最小内存地址为: 0x" << SysInfo.lpMinimumApplicationAddress << endl;
	cout << "每个进程可用的私有地址空间的最大内存地址为: 0x" << SysInfo.lpMaximumApplicationAddress << endl;
	cout << "能够保留地址空间区域的最小单位为: " << SysInfo.dwAllocationGranularity / 1024 << "KB" << endl;
	cout << "------------------------------------------------------" << endl;
}

void ShowPerformanceInfo(PERFORMACE_INFORMATION PerforInfo)
{
	cout << "----------------系统的存储器使用情况------------------" << endl;
	cout << "结构体的大小为: " << PerforInfo.cb << "B" << endl;
	cout << "系统当前提交的页面总数: " << PerforInfo.CommitTotal << endl;
	cout << "系统当前可提交的最大页面总数: " << PerforInfo.CommitLimit << endl;
	cout << "自上次系统重新引导以来同时处于已提交状态的最大页数: " << PerforInfo.CommitPeak << endl;
	cout << "总物理内存: " << PerforInfo.PhysicalTotal << "页" << endl;
	cout << "当前可用的物理内存为: " << PerforInfo.PhysicalAvailable << "页" << endl;
	cout << "系统Cache的容量为: " << PerforInfo.SystemCache << "页" << endl;
	cout << "内存总量为: " << PerforInfo.KernelTotal << "页" << endl;
	cout << "分页池的大小为: " << PerforInfo.KernelPaged << "页" << endl;
	cout << "非分页池的大小为: " << PerforInfo.KernelNonpaged << "页" << endl;
	cout << "页的大小为: " << PerforInfo.PageSize << "B" << endl;
	cout << "打开的句柄个数为: " << PerforInfo.HandleCount << endl;
	cout << "进程个数为: " << PerforInfo.ProcessCount << endl;
	cout << "线程个数为: " << PerforInfo.ThreadCount << endl;
	cout << "------------------------------------------------------" << endl;
}

void GetProcessInfo()
{
	cout << "------------------各个进程的信息----------------------" << endl;

	//Windows使用PROCESSENTRY32结构体描述在拍摄快照时驻留在系统地址空间中的进程列表中的条目
	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(ProcessEntry);
	HANDLE ProcessSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(ProcessSnapHandle, &ProcessEntry))
	{
		do
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID);
			//获取进程内存信息，这些信息存储在PROCESS_MEMORY_COUNTERS结构体中
			PROCESS_MEMORY_COUNTERS PMC;
			ZeroMemory(&PMC, sizeof(PMC));

			if (GetProcessMemoryInfo(ProcessHandle, &PMC, sizeof(PMC)))
			{
				cout << "进程ID: ";
				wcout<< ProcessEntry.th32ProcessID << endl;
				cout << "进程名: ";
				wcout << ProcessEntry.szExeFile << endl;
				cout << "进程启动的线程数：";
				wcout << ProcessEntry.cntThreads << endl;
				cout << "虚拟内存大小: " << (float)PMC.WorkingSetSize / 1024 << "KB" << endl;
				cout << "------------------------------------------------------" << endl;
			}
		} while (Process32Next(ProcessSnapHandle, &ProcessEntry));
	}
}

void ShowProcessMemory(HANDLE ProcessHandle)
{
	SYSTEM_INFO SysInfo;
	ZeroMemory(&SysInfo, sizeof(SysInfo));
	GetSystemInfo(&SysInfo);

	MEMORY_BASIC_INFORMATION MBI;
	ZeroMemory(&MBI, sizeof(MBI));

	//循环整个应用程序地址空间

	//每个进程可用地址空间的最小内存地址
	LPCVOID ProcessMiniAddress = (LPVOID)SysInfo.lpMinimumApplicationAddress;
	while (ProcessMiniAddress < SysInfo.lpMaximumApplicationAddress)
	{
		//获得虚拟内存块信息
		//VirtualQueryEx("进程句柄","查询的页的基地址","缓冲区接收查询信息","缓冲区大小")
		if (VirtualQueryEx(ProcessHandle, ProcessMiniAddress, &MBI, sizeof(MBI)))
		{
			//计算虚拟内存块的长度
			LPCVOID ProcessMaxAddress = (PBYTE)ProcessMiniAddress + MBI.RegionSize;
			TCHAR BlockSize[MAX_PATH];
			StrFormatByteSize64(MBI.RegionSize, BlockSize, MAX_PATH);

			cout.fill('0');
			cout << "OX" << setw(8) << hex << (DWORD)ProcessMiniAddress << "-";
			cout << "OX" << setw(8) << hex << (DWORD)ProcessMaxAddress << "(";
			

			
			printf("%ls)", BlockSize);

			//显示块的状态
			switch (MBI.State)
			{
			case MEM_COMMIT:
				cout << "COMMIT ";
				break;
			case MEM_FREE:
				cout << "FREE   ";
				break;
			case MEM_RESERVE:
				cout << "RESERVE";
				break;
			default:
				break;
			}
			cout << "    ";
			//显示虚拟内存块的保护属性
			cout.fill(' ');
			switch (MBI.Protect)
			{
			case PAGE_NOACCESS:
				cout <<setw(20)<< "NOACCESS";
				break;
			case PAGE_READONLY:
				cout << setw(20) << "READONLY";
				break;
			case PAGE_EXECUTE:
				cout << setw(20) << "EXCUTE";
				break;
			case PAGE_EXECUTE_READ:
				cout << setw(20) << "EXCUTE_READ";
				break;
			case PAGE_EXECUTE_READWRITE:
				cout << setw(20) << "EXCUTE_READ_WRITE";
				break;
			case PAGE_EXECUTE_WRITECOPY:
				cout << setw(20) << "EXCUTE_WRITE_COPY";
				break;
			case PAGE_GUARD:
				cout << setw(20) << "GUARD";
				break;
			case PAGE_NOCACHE:
				cout << setw(20) << "NOCACHE";
				break;
			case PAGE_READWRITE:
				cout << setw(20) << "READ_WRITE";
				break;
			case PAGE_WRITECOPY:
				cout << setw(20) << "WRITE_COPY";
				break;
			default:
				cout << setw(20) << "ANOTHER";
				break;
			}
			cout << "    ";
			//显示存储类型
			switch (MBI.Type)
			{
			case MEM_IMAGE:
				cout << "IMAGE  ";
				break;
			case MEM_MAPPED:
				cout << "MAPPED ";
				break;
			case MEM_PRIVATE:
				cout << "PRIVATE";
				break;
			default:
				break;
			}
			cout << "    ";
			//定义字符数组用来存放文件路径，MAX_PATH为宏定义=260
			//TCHAR为适用于UNICODE和ANSI编码的字符变量，可以自适应调整
			TCHAR filePath[MAX_PATH];
			//获得当前内存卡执行程序的路径
			if (GetModuleFileName((HMODULE)ProcessMiniAddress, filePath, MAX_PATH) > 0)
			{
				PathStripPath(filePath);
				wcout << filePath;
			}
			cout << endl;
			//移动到下一个块
			ProcessMiniAddress = ProcessMaxAddress;
		}
	}

}

int main(int argc, char *argv[])
{
	//申明系统内存状态结构，存储系统内存信息
	MEMORYSTATUSEX Statusex;
	Statusex.dwLength = sizeof(Statusex);
	//获取系统内存信息
	GlobalMemoryStatusEx(&Statusex);
	ShowSystemMemoryStatus(Statusex);

	//申明系统信息结构
	SYSTEM_INFO SysInfo;
	ZeroMemory(&SysInfo, sizeof(SysInfo));
	//获取系统信息
	GetSystemInfo(&SysInfo);
	ShowSystemInfo(SysInfo);

	//申明系统存储器结构，存储系统存储器使用情况
	PERFORMACE_INFORMATION PerforInfo;
	PerforInfo.cb = sizeof(PerforInfo);
	//获取系统存储器使用情况
	GetPerformanceInfo(&PerforInfo, PerforInfo.cb);
	ShowPerformanceInfo(PerforInfo);

	//获取进程列表
	GetProcessInfo();

	//查询具体进程虚拟空间布局和工作集信息
	cout << "-------进程虚拟地址空间布局和工作集信息查询---------" << endl;
	int ProcessID = 0;
	while (1)
	{
		cout << "输入要查询的进程ID（输入-1退出）：";
		cin >> ProcessID;
		if (ProcessID == -1)
		{
			break;
		}
		//打开相关进程
		HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
		if (ProcessHandle != NULL)
		{
			ShowProcessMemory(ProcessHandle);
		}
		else
		{
//			CloseHandle(ProcessHandle);
			cout << "输入进程ID不存在，请重新输入" << endl;
		}
	}
}