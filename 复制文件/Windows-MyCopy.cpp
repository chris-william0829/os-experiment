#include<stdio.h>
#include<string.h>
#include<fstream>
#include<string>
#include<iostream>
#include<Windows.h>

using namespace std;



void MyCopy(const char SourcePath[], const char TargetPath[])
{
	WIN32_FIND_DATA FindFileData;
	if (FindFirstFile(SourcePath, &FindFileData) == INVALID_HANDLE_VALUE)
	{
		printf("����Դ�ļ�·��ʧ��!\n");
		return;
	}
	//��dwFileAttributes��FILE_ATTRIBUTE_DIRECTORY��λ�������� ���ж����ҵ�����Ŀ�ǲ����ļ���
	//��������ļ��У�ֱ�Ӹ���
	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		//Windows�Դ��ļ�����API��FALSE��������ļ������򸲸��ļ�
		CopyFile(SourcePath, TargetPath, FALSE);
		
		return;
	}
	//������ļ��У������ļ���
	CreateDirectory(TargetPath, NULL);
	char SourceDirectory[MAX_PATH];
	sprintf_s(SourceDirectory, "%s\\*", SourcePath);
	HANDLE FileHandle = FindFirstFile(SourceDirectory, &FindFileData);
	for (BOOL NextFile = TRUE; NextFile; NextFile = FindNextFile(FileHandle, &FindFileData))
	{
		char *FileName = FindFileData.cFileName;
		//����ļ�����Ŀ¼�ļ�������
		if (strcmp(FileName, ".") == 0)
		{
			continue;
		}
		if (strcmp(FileName, "..") == 0)
		{
			continue;
		}
		//������ļ������ļ�����·���������ӣ��ݹ���и���
		char SourceSubPath[MAX_PATH];
		sprintf_s(SourceSubPath, "%s\\%s", SourcePath, FileName);
		char TargetSubPath[MAX_PATH];
		sprintf_s(TargetSubPath, "%s\\%s", TargetPath, FileName);
		MyCopy(SourceSubPath, TargetSubPath);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("������Դ�ļ���Ŀ���ļ�\n");
		return 0;
	}
	else
	{
		MyCopy(argv[1], argv[2]);
		printf("�������!\n");
		return 0;
	}
}