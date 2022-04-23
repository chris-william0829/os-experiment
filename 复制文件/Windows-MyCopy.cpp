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
		printf("查找源文件路径失败!\n");
		return;
	}
	//将dwFileAttributes和FILE_ATTRIBUTE_DIRECTORY做位的与运算 来判断所找到的项目是不是文件夹
	//如果不是文件夹，直接复制
	if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		//Windows自带文件复制API，FALSE代表如果文件存在则覆盖文件
		CopyFile(SourcePath, TargetPath, FALSE);
		
		return;
	}
	//如果是文件夹，创建文件夹
	CreateDirectory(TargetPath, NULL);
	char SourceDirectory[MAX_PATH];
	sprintf_s(SourceDirectory, "%s\\*", SourcePath);
	HANDLE FileHandle = FindFirstFile(SourceDirectory, &FindFileData);
	for (BOOL NextFile = TRUE; NextFile; NextFile = FindNextFile(FileHandle, &FindFileData))
	{
		char *FileName = FindFileData.cFileName;
		//如果文件名是目录文件，跳过
		if (strcmp(FileName, ".") == 0)
		{
			continue;
		}
		if (strcmp(FileName, "..") == 0)
		{
			continue;
		}
		//如果是文件，将文件名与路径进行连接，递归进行复制
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
		printf("请输入源文件和目标文件\n");
		return 0;
	}
	else
	{
		MyCopy(argv[1], argv[2]);
		printf("复制完成!\n");
		return 0;
	}
}