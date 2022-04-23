#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
using namespace std;
typedef struct _UseTime
{
	int ms;
	int second;
	int minute;
	int hour;
	int day;
}UseTime;
void timeprocess(UseTime &t,SYSTEMTIME &StartTime,SYSTEMTIME &EndTime)
{
	t.ms = EndTime.wMilliseconds - StartTime.wMilliseconds;
	t.second = EndTime.wSecond - StartTime.wSecond;
	t.minute = EndTime.wMinute - StartTime.wMinute;
	t.hour = EndTime.wHour - StartTime.wHour;
	t.day = EndTime.wDay - StartTime.wDay;
	if (t.ms < 0){t.ms += 1000;t.second -= 1;}
	if (t.second < 0){t.second += 60;t.minute -= 1;}
	if (t.minute < 0) { t.minute += 60; t.hour -= 1; }
	if (t.hour < 0) { t.hour += 24; t.day -= 1; }
}

int main(char argc, char *argv[])
{
	SYSTEMTIME StartTime, EndTime;
	STARTUPINFO si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	//si.lpReserved = NULL;
	UseTime t;
	PROCESS_INFORMATION pi;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	//si.wShowWindow = SW_SHOW;

	if (!(CreateProcess(NULL, argv[1], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)))
	{
		DWORD ErrorCode = GetLastError();
		cout << ErrorCode << endl;
	}
	else
	{
		GetSystemTime(&StartTime);
		cout << "StartTime:" << StartTime.wYear << ":" << StartTime.wMonth << ":" << StartTime.wDay << "-" << StartTime.wHour << ":" << StartTime.wMinute << ":" << StartTime.wSecond << ":" << StartTime.wMilliseconds << endl;

	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetSystemTime(&EndTime);
	cout << "EndTime:" << EndTime.wYear << ":" << EndTime.wMonth << ":" << EndTime.wDay << "-" << EndTime.wHour << ":" << EndTime.wMinute << ":" << EndTime.wSecond << ":" << EndTime.wMilliseconds << endl;
	timeprocess(t, StartTime, EndTime);
	cout << "ChildProcess Used:"<<t.minute << "Minute " << t.second << "seconds " << t.ms << "ms";
	system("pause");
	return 0;
}