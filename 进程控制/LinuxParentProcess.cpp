#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<sys/time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<math.h>
using namespace std;
int main(char agrc,char **argv)
{
    struct timeval StartTime,EndTime;
    float UseTime=0;
    pid_t pid;
    pid=fork();
    if(pid<0)
    {
        cout<<"Create ChildProcess Failed"<<endl;
        exit(0);
    }
    else if(pid==0)
    {
        gettimeofday(&StartTime,NULL);
        cout<<"ChildProcess Start at :"<<StartTime.tv_sec<<"ms "<<StartTime.tv_usec<<"us"<<endl;
        execv(argv[1],&argv[1]);
    }
    else
    {
        wait(nullptr);
        gettimeofday(&EndTime,NULL);
        cout<<"Process End at :"<<EndTime.tv_sec<<"ms "<<EndTime.tv_usec<<"us"<<endl;
        UseTime=(EndTime.tv_sec-StartTime.tv_sec)*1000000+(EndTime.tv_usec-StartTime.tv_usec);
        cout<<"This process Used:"<<UseTime/1000<<"ms"<<endl;

    }
    return 0;
    
}