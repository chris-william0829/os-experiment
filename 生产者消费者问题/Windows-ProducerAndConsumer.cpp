#include<iostream>
#include<Windows.h>
#include<time.h>
#include<string.h>

using namespace std;

//����HANDLE���������洢�����������������߽��̾��
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
	//���һ��3000ms���ڵ�����������
	int randomTime = rand() % 3000;
	Sleep(randomTime);
}

char randChar()
{
	//���һ����дӢ����ĸ��Ϊ�����������Ĳ�Ʒ
	return Name[rand() % 4];
}

//�������̶���
void createProcess(int id)
{
	//�����ַ�������������ļ�·����MAX_PATHΪ�궨��=260
	//TCHARΪ������UNICODE��ANSI������ַ���������������Ӧ����
	TCHAR filePath[MAX_PATH];
	//��õ�ǰִ�г����·��
	GetModuleFileName(NULL, filePath, MAX_PATH);
	TCHAR cmd[MAX_PATH];
	sprintf_s(cmd, "\"%s\" %d", filePath, id);
	//cout << cmd << endl;
	//����STARTUPINFO�ṹ��������ָ���½����Ӵ������ԣ������г�ʼ��
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	//����PROCESS_INFORMATION�ṹ������¼�½��̵���Ϣ��
	PROCESS_INFORMATION pi;

	/*
	 1).��һ������Ϊ�ӽ��̵��ļ���
	 2).�ڶ�������Ϊ�����в���
	 3).����������ָ��ΪNULL��ΪĬ�ϵĽ��̰�ȫ��������
	 4).���ĸ�����ָ��ΪNULL��ΪĬ�ϵ��̰߳�ȫ��������
	 5).���������ָ��ΪFALSE�������ӽ��̲��ɼ̳е�ǰ���̵ľ����
	 6).����������ָ��Ϊ0����ʾ�ӽ��̲������¿���̨���ڡ�
	 7).���߸�����ָ��ΪNULL����ʾ�ӽ���ʹ�õ�ǰ���̵Ļ�����
	 8).�ڰ˸�����ָ��ΪNULL����ʾ�ӽ���ʹ�õ�ǰ���̵ĵ���������Ŀ¼��
	 9).�ھŸ�����Ϊ�����ӽ��̵������������ʾ�Ľṹ�壬�˴�Ϊ�ϲ������si��
	 10).��ʮ������Ϊ&pi��Ϊ�����½�����Ϣ�Ľṹ�塣
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
	//���������ź���
	//HANDLE mutex = CreateSemaphore(NULL, 1, 1, "MYMUTEX");
	//CreateMutex("��ȫ����","�Ƿ�ռ��","����")
	HANDLE mutex = CreateMutex(NULL, FALSE, "MUTEX");
	//�����ź���
	HANDLE empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
	HANDLE full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");

	//���ļ�ӳ��������ھ���У�������
	//OpenFileMapping("����ģʽ" "�̳б�־" "�ļ�ӳ�������ָ��")
	HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FILEMAP");
	//MapViewOfFile(�ļ�ӳ�������������ģʽ���ļ�ƫ�Ƹ�32λ���ļ�ƫ�Ƶ�32λ��ӳ����ͼ��С) Ϊ0ӳ�������ļ�
	LPVOID data = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	//ǿ������ת��
	buffer *bufferData = reinterpret_cast<buffer *>(data);

	//�ظ�4��
	for (int i = 0; i < 4; i++)
	{
		//P����������empty�ź���
		WaitForSingleObject(empty, INFINITE);
		//�������3s���ڵ�ʱ��
		sleepRandomTime();

		//P���������뻥���ź��������ڻ���ֻ��һ�����̷��ʻ�����
		WaitForSingleObject(mutex, INFINITE);

		//�������������Ʒ
		char product = randChar();
		bufferData->buffer[bufferData->write] = product;
		bufferData->write = (bufferData->write + 1) % 4;

		//�ͷ��ź������������ͷ�full�����ͷ�mutex
		ReleaseSemaphore(full, 1, NULL);
		ReleaseMutex(mutex);

		printf("������%d�򻺳���д������%c\n", (int)GetCurrentProcessId(),product);
		fflush(stdout);
		printf("������:\t%c\t%c\t%c\t%c\n", bufferData->buffer[0], bufferData->buffer[1], bufferData->buffer[2], bufferData->buffer[3]);
		fflush(stdout);
	}
	//����ļ�ӳ��
	UnmapViewOfFile(data);
	data = NULL;
	CloseHandle(mutex);
	CloseHandle(full);
	CloseHandle(empty);
}

void consumer()
{
	//���������ź���
	//HANDLE mutex = CreateSemaphore(NULL, 1, 1, "MYMUTEX");
	//CreateMutex("��ȫ����","�Ƿ�ռ��","����")
	HANDLE mutex = CreateMutex(NULL, FALSE, "MUTEX");
	//�����ź���
	HANDLE empty = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "EMPTY");
	HANDLE full = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "FULL");

	//���ļ�ӳ��������ھ����
	//OpenFileMapping("����ģʽ" "�̳б�־" "�ļ�ӳ�������ָ��")
	HANDLE hMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FILEMAP");
	//MapViewOfFile(�ļ�ӳ�������������ģʽ���ļ�ƫ�Ƹ�32λ���ļ�ƫ�Ƶ�32λ��ӳ����ͼ��С) Ϊ0ӳ�������ļ�
	LPVOID data = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	//ǿ������ת��
	buffer *bufferData = reinterpret_cast<buffer *>(data);
	for (int i = 0; i < 3; i++)
	{
		//P����������full�ź������ʻ�����
		WaitForSingleObject(full, INFINITE);
		//�������3s���ڵ�ʱ��
		sleepRandomTime();
		//P����������mutex��ִ��ȡ��Ʒ����
		WaitForSingleObject(mutex, INFINITE);

		char product = bufferData->buffer[bufferData->read];
		bufferData->buffer[bufferData->read] = '-';
		bufferData->read = (bufferData->read + 1) % 4;

		//V������ͬ�����ܽ���˳��
		ReleaseSemaphore(empty,1,NULL);
		ReleaseMutex(mutex);
		
		
		printf("������%d�򻺳���ȡ������%c\n", (int)GetCurrentProcessId(),product);
		fflush(stdout);
		printf("������:\t%c\t%c\t%c\t%c\n", bufferData->buffer[0], bufferData->buffer[1], bufferData->buffer[2], bufferData->buffer[3]);
		fflush(stdout);
	}
	//����ļ�ӳ��
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
		//CreateFileMapping("�����ļ����","��ȫ����","��������","�ļ�ӳ�����󳤶ȵĸ�32λ","�ļ�ӳ�����󳤶ȵĵ�32λ","�ļ�ӳ�������")
		//��ҳʽ��ʱ�ļ�����һ���ļ�ӳ�����򣬴�СλBUFFER�Ļ�����FILEMAP
		HANDLE hMap = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, sizeof(buffer), "FILEMAP");

		if (hMap != INVALID_HANDLE_VALUE)
		{
			//MapViewOfFile(�ļ�ӳ�������������ģʽ���ļ�ƫ�Ƹ�32λ���ļ�ƫ�Ƶ�32λ��ӳ����ͼ��С) Ϊ0ӳ�������ļ�
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

		//CreateSemaphore("�ź�������","��ʼֵ","���ֵ","����")
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