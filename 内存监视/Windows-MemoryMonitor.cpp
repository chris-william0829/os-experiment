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

	cout << "-----------------------�ڴ���Ϣ-----------------------" << endl;
	//�ڴ�ʹ����
	cout << "�����ڴ��ʹ����Ϊ: " << Statusex.dwMemoryLoad << "%%" << endl;
	//�����ڴ�
	cout << "�����ڴ��������Ϊ: " << ((float)Statusex.ullTotalPhys / 1024 / 1024 / 1024) << "GB." << endl;
	//���������ڴ�
	cout << "���õ������ڴ�Ϊ: " << ((float)Statusex.ullAvailPhys / 1024 / 1024 / 1024) << "GB." << endl;
	//�ύ���ڴ�����
	cout << "�ܵĽ����ļ�Ϊ: " << ((float)Statusex.ullTotalPageFile / 1024 / 1024 / 1024) << "GB." << endl;
	//��ǰ���̿����ύ������ڴ���
	cout << "���õĽ����ļ�Ϊ��" << ((float)Statusex.ullAvailPageFile / 1024 / 1024 / 1024) << "GB." << endl;
	//�����ڴ�
	cout << "�����ڴ��������Ϊ��" << ((float)Statusex.ullTotalVirtual / 1024 / 1024 / 1024) << "GB." << endl;
	//���������ڴ�
	cout << "���õ������ڴ�Ϊ��" << ((float)Statusex.ullAvailVirtual / 1024 / 1024 / 1024) << "GB." << endl;
	//�����ֶ�
	cout << "�����ֶε�����Ϊ��" << Statusex.ullAvailExtendedVirtual << "Byte." << endl;
	cout << "------------------------------------------------------" << endl;
}

void ShowSystemInfo(SYSTEM_INFO SysInfo)
{
	cout << "---------------------ϵͳ��Ϣ-------------------------" << endl;
	cout << "�ڴ�ҳ�Ĵ�СΪ��" << (int)SysInfo.dwPageSize / 1024 << "KB.";
	cout << "ÿ�����̿��õ�ַ�ռ����С�ڴ��ַΪ: 0x" << SysInfo.lpMinimumApplicationAddress << endl;
	cout << "ÿ�����̿��õ�˽�е�ַ�ռ������ڴ��ַΪ: 0x" << SysInfo.lpMaximumApplicationAddress << endl;
	cout << "�ܹ�������ַ�ռ��������С��λΪ: " << SysInfo.dwAllocationGranularity / 1024 << "KB" << endl;
	cout << "------------------------------------------------------" << endl;
}

void ShowPerformanceInfo(PERFORMACE_INFORMATION PerforInfo)
{
	cout << "----------------ϵͳ�Ĵ洢��ʹ�����------------------" << endl;
	cout << "�ṹ��Ĵ�СΪ: " << PerforInfo.cb << "B" << endl;
	cout << "ϵͳ��ǰ�ύ��ҳ������: " << PerforInfo.CommitTotal << endl;
	cout << "ϵͳ��ǰ���ύ�����ҳ������: " << PerforInfo.CommitLimit << endl;
	cout << "���ϴ�ϵͳ������������ͬʱ�������ύ״̬�����ҳ��: " << PerforInfo.CommitPeak << endl;
	cout << "�������ڴ�: " << PerforInfo.PhysicalTotal << "ҳ" << endl;
	cout << "��ǰ���õ������ڴ�Ϊ: " << PerforInfo.PhysicalAvailable << "ҳ" << endl;
	cout << "ϵͳCache������Ϊ: " << PerforInfo.SystemCache << "ҳ" << endl;
	cout << "�ڴ�����Ϊ: " << PerforInfo.KernelTotal << "ҳ" << endl;
	cout << "��ҳ�صĴ�СΪ: " << PerforInfo.KernelPaged << "ҳ" << endl;
	cout << "�Ƿ�ҳ�صĴ�СΪ: " << PerforInfo.KernelNonpaged << "ҳ" << endl;
	cout << "ҳ�Ĵ�СΪ: " << PerforInfo.PageSize << "B" << endl;
	cout << "�򿪵ľ������Ϊ: " << PerforInfo.HandleCount << endl;
	cout << "���̸���Ϊ: " << PerforInfo.ProcessCount << endl;
	cout << "�̸߳���Ϊ: " << PerforInfo.ThreadCount << endl;
	cout << "------------------------------------------------------" << endl;
}

void GetProcessInfo()
{
	cout << "------------------�������̵���Ϣ----------------------" << endl;

	//Windowsʹ��PROCESSENTRY32�ṹ���������������ʱפ����ϵͳ��ַ�ռ��еĽ����б��е���Ŀ
	PROCESSENTRY32 ProcessEntry;
	ProcessEntry.dwSize = sizeof(ProcessEntry);
	HANDLE ProcessSnapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(ProcessSnapHandle, &ProcessEntry))
	{
		do
		{
			HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessEntry.th32ProcessID);
			//��ȡ�����ڴ���Ϣ����Щ��Ϣ�洢��PROCESS_MEMORY_COUNTERS�ṹ����
			PROCESS_MEMORY_COUNTERS PMC;
			ZeroMemory(&PMC, sizeof(PMC));

			if (GetProcessMemoryInfo(ProcessHandle, &PMC, sizeof(PMC)))
			{
				cout << "����ID: ";
				wcout<< ProcessEntry.th32ProcessID << endl;
				cout << "������: ";
				wcout << ProcessEntry.szExeFile << endl;
				cout << "�����������߳�����";
				wcout << ProcessEntry.cntThreads << endl;
				cout << "�����ڴ��С: " << (float)PMC.WorkingSetSize / 1024 << "KB" << endl;
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

	//ѭ������Ӧ�ó����ַ�ռ�

	//ÿ�����̿��õ�ַ�ռ����С�ڴ��ַ
	LPCVOID ProcessMiniAddress = (LPVOID)SysInfo.lpMinimumApplicationAddress;
	while (ProcessMiniAddress < SysInfo.lpMaximumApplicationAddress)
	{
		//��������ڴ����Ϣ
		//VirtualQueryEx("���̾��","��ѯ��ҳ�Ļ���ַ","���������ղ�ѯ��Ϣ","��������С")
		if (VirtualQueryEx(ProcessHandle, ProcessMiniAddress, &MBI, sizeof(MBI)))
		{
			//���������ڴ��ĳ���
			LPCVOID ProcessMaxAddress = (PBYTE)ProcessMiniAddress + MBI.RegionSize;
			TCHAR BlockSize[MAX_PATH];
			StrFormatByteSize64(MBI.RegionSize, BlockSize, MAX_PATH);

			cout.fill('0');
			cout << "OX" << setw(8) << hex << (DWORD)ProcessMiniAddress << "-";
			cout << "OX" << setw(8) << hex << (DWORD)ProcessMaxAddress << "(";
			

			
			printf("%ls)", BlockSize);

			//��ʾ���״̬
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
			//��ʾ�����ڴ��ı�������
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
			//��ʾ�洢����
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
			//�����ַ�������������ļ�·����MAX_PATHΪ�궨��=260
			//TCHARΪ������UNICODE��ANSI������ַ���������������Ӧ����
			TCHAR filePath[MAX_PATH];
			//��õ�ǰ�ڴ濨ִ�г����·��
			if (GetModuleFileName((HMODULE)ProcessMiniAddress, filePath, MAX_PATH) > 0)
			{
				PathStripPath(filePath);
				wcout << filePath;
			}
			cout << endl;
			//�ƶ�����һ����
			ProcessMiniAddress = ProcessMaxAddress;
		}
	}

}

int main(int argc, char *argv[])
{
	//����ϵͳ�ڴ�״̬�ṹ���洢ϵͳ�ڴ���Ϣ
	MEMORYSTATUSEX Statusex;
	Statusex.dwLength = sizeof(Statusex);
	//��ȡϵͳ�ڴ���Ϣ
	GlobalMemoryStatusEx(&Statusex);
	ShowSystemMemoryStatus(Statusex);

	//����ϵͳ��Ϣ�ṹ
	SYSTEM_INFO SysInfo;
	ZeroMemory(&SysInfo, sizeof(SysInfo));
	//��ȡϵͳ��Ϣ
	GetSystemInfo(&SysInfo);
	ShowSystemInfo(SysInfo);

	//����ϵͳ�洢���ṹ���洢ϵͳ�洢��ʹ�����
	PERFORMACE_INFORMATION PerforInfo;
	PerforInfo.cb = sizeof(PerforInfo);
	//��ȡϵͳ�洢��ʹ�����
	GetPerformanceInfo(&PerforInfo, PerforInfo.cb);
	ShowPerformanceInfo(PerforInfo);

	//��ȡ�����б�
	GetProcessInfo();

	//��ѯ�����������ռ䲼�ֺ͹�������Ϣ
	cout << "-------���������ַ�ռ䲼�ֺ͹�������Ϣ��ѯ---------" << endl;
	int ProcessID = 0;
	while (1)
	{
		cout << "����Ҫ��ѯ�Ľ���ID������-1�˳�����";
		cin >> ProcessID;
		if (ProcessID == -1)
		{
			break;
		}
		//����ؽ���
		HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
		if (ProcessHandle != NULL)
		{
			ShowProcessMemory(ProcessHandle);
		}
		else
		{
//			CloseHandle(ProcessHandle);
			cout << "�������ID�����ڣ�����������" << endl;
		}
	}
}