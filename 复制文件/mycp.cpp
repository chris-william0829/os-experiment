#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<dirent.h>
#include<sys/stat.h>

void CopyFile(char SourceFile[], char TargetFile[])
{
	int SourceFileData = open(SourceFile, O_RDONLY);
	int TargetFileData = open(TargetFile, O_WRONLY | O_CREAT, 0666);
	char Buffer[1024];
	ssize_t Bytes;
	while ((Bytes = read(SourceFileData, Buffer, sizeof(Buffer)) > 0)
	{
		write(TargetFileData, Buffer, Bytes);
	}
	close(SourceFileData);
	close(TargetFileData);
}

void MyCopy(char SourcePath[], char TargetPath[])
{
	struct stat StatBuffer;
	lstat(SourcePath, &StatBuffer);

	if (S_ISLNK(StatBuffer.st_mode))
	{
		char Buffer[MAXNAMLEN];
		readlink(SourcePath, Buffer, sizeof(Buffer));
		symlink(Buffer, TargetPath);
		return;
	}
	if (!S_ISDIR(StatBuffer.st_mode))
	{
		CopyFile(SourcePath, TargetPath);
		return;
	}
	mkdir(TargetPath, StatBuffer.st_mode);
	DIR *SourceDirectory = opendir(SourcePath);
	struct dirent *SourceDirent;
	while ((SourceDirent = readdir(sourceDirectory)) != NULL)
	{
		char *FileName = SourceDirent->d_name;
		if (strcmp(FileName, ".") == 0)
		{
			continue;
		}
		if (strcmp(FileName, "..") == 0)
		{
			continue;
		}
		char SourceSubPath[MAXNAMLEN];
		sprintf_s(SourceSubPath, "%s\\%s", SourcePath, FileName);
		char TargetSubPath[MAXNAMLEN];
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