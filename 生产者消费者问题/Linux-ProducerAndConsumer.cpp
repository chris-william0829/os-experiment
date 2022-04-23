//Linux                                                                                                                                                                                       
#include<string.h>
#include <sys/time.h>
#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/sem.h>
#include<sys/select.h>
#include<sys/wait.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<time.h>

#define semMutexID 225

#define semEmptyID 97

#define semFullID 234

#define shareMemoryKey 75

char Name[4]{'C','H','E','N'};

typedef struct _buffer
{
	char buffer[4];
	int read;
	int write;
}buffer;

void randomSleep()
{
	sleep(rand()%3);
}

char randomProduce()
{
	return Name[(rand()%4)];
}
void P(int s)
{
	struct sembuf sem_op;
	//singal Num
	sem_op.sem_num = 0;
	//singal operate always(-1,+1)P,V
	sem_op.sem_op = -1;
	sem_op.sem_flg = 0;
	semop(s, &sem_op, 1);
}
void V(int s)
{
	struct sembuf sem_op;
	sem_op.sem_num = 0;
	sem_op.sem_op = 1;
	sem_op.sem_flg = 0;
	semop(s, &sem_op, 1);
}
void producer()
{
	int shareMemoryID;
	int semMutex, semEmpty, semFull;
	void * memoryAddress;
	buffer *bufferData;
	struct sembuf semOP;
	struct timeval systemTime;
	//Create or get(if exist) the semaphore by the key SEM_ID1
	//semget("key to the semaphore","num to create","privilage")
	semMutex = semget(semMutexID, 1, 0600);
	semEmpty = semget(semEmptyID, 1, 0600);
	semFull = semget(semFullID, 1, 0600);
	//to get the ID of the share memory
	shareMemoryID = shmget(shareMemoryKey, sizeof(buffer), 0777);
	//make the memory into the process while running
	memoryAddress = shmat(shareMemoryID, 0, 0);
	//make the memory transform to the bufferData
	bufferData=(buffer *)memoryAddress;
	for(int i=0;i<4;i++)
	{
		//P(empty)
		P(semEmpty);
		gettimeofday(&systemTime, NULL);
		srand((unsigned)systemTime.tv_usec);
		randomSleep();

		//P(mutex)
		P(semMutex);

		char product = randomProduce();
		bufferData->buffer[bufferData->write] = product;
		bufferData->write = (bufferData->write + 1) % 4;

		printf("Producer%d produce %c\n",(int)getpid(),product);

		printf("The Buffer:\t%c\t%c\t%c\t%c\n",bufferData->buffer[0],bufferData->buffer[1],bufferData->buffer[2],bufferData->buffer[3]);

		//V(full)
		V(semFull);
		//V(mutex)
		V(semMutex);
	}
	//relieve the mapping of share memory
	shmdt(memoryAddress);
}
void consumer()
{
	int shareMemoryID;
	int semMutex, semEmpty, semFull;
	void * memoryAddress;
	buffer *bufferData;
	struct sembuf semOP;
	struct timeval systemTime;
	//Create or get(if exist) the semaphore by the key SEM_ID1
	//semget("key to the semaphore","num to create","privilage")
	semMutex = semget(semMutexID, 1, 0600);
	semEmpty = semget(semEmptyID, 1, 0600);
	semFull = semget(semFullID, 1, 0600);
	//to get the ID of the share memory
	shareMemoryID = shmget(shareMemoryKey, sizeof(buffer), 0777);
	//make the memory into the process while running
	memoryAddress = shmat(shareMemoryID, 0, 0);
	//make the memory transform to the bufferData
	bufferData=(buffer *)memoryAddress;
	for(int i=0;i<3;i++)
	{
		//P(full)
		P(semFull);
		gettimeofday(&systemTime, NULL);
		srand((unsigned)systemTime.tv_usec);
		randomSleep();

		//P(mutex)
		P(semMutex);

		char product = bufferData->buffer[bufferData->read];
		bufferData->buffer[bufferData->read] = '-';
		bufferData->read = (bufferData->read + 1) % 4;

		printf("Consumer%d take %c\n",(int)getpid(),product);

		printf("The Buffer:\t%c\t%c\t%c\t%c\n",bufferData->buffer[0],bufferData->buffer[1],bufferData->buffer[2],bufferData->buffer[3]);

		//V(empty)
		V(semEmpty);
		//V(mutex)
		V(semMutex);
	}
	//relieve the mapping of share memory
	shmdt(memoryAddress);
}

int main()
{
	int semMutex, semEmpty, semFull, shareMemoryID;
	void * memoryAddress;
	union semun {
		int val;
	}empty, full, mutex;

	semMutex = semget(semMutexID, 1, IPC_CREAT | 0600);
	semEmpty = semget(semEmptyID, 1, IPC_CREAT | 0600);
	semFull = semget(semFullID, 1, IPC_CREAT | 0600);

	full.val = 0;
	empty.val = 4;
	mutex.val = 1;

	//initial the semaphore
	//semctl(id,the number in the set of sem,cmd,[val])
	//SETVAL set val = xxxxx
	semctl(semMutex, 0, SETVAL, mutex);
	semctl(semEmpty, 0, SETVAL, empty);
	semctl(semFull, 0, SETVAL, full);

	//to get the ID of the share memory
	// shmget(key,size,privilage)
	shareMemoryID = shmget(shareMemoryKey, sizeof(buffer), 0777 | IPC_CREAT);
	if (-1 == shareMemoryID)
	{
		printf("Memory Share Failed\n");
		exit(0);
	}
	//make the memory mapping into the process while running
	memoryAddress = shmat(shareMemoryID, 0, 0);
	memset(memoryAddress, 0, sizeof(buffer));
	buffer *bufferData=(buffer *)memoryAddress;
	memset(bufferData->buffer,'-',sizeof(bufferData->buffer));
	bufferData->read=0;
	bufferData->write=0;
	//execute the producer process
	for (int i = 0; i < 3; i++)
	{
		if (fork() == 0)
		{
			producer();
			exit(0);
		}
	}
	//execute the consumer process
	for (int i = 0; i < 4; i++)
	{
		if (fork() == 0)
		{
			consumer();
			exit(0);
		}
	}
	//tie while all the process die
	while (-1 != wait(0));
	
	//IPC_RMID:delete sem from memory
	semctl(semMutex, 0, IPC_RMID);
	semctl(semEmpty, 0, IPC_RMID);
	semctl(semFull, 0, IPC_RMID);
	shmdt(memoryAddress);
}