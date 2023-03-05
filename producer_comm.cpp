#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "stdio.h"
#include <cstdio>
#include <cstdint>
#include <errno.h> 
#include <stdlib.h> 
#include <cstring>
#include <sys/types.h> 
#include <sys/sem.h>
#include<unistd.h>
#include <string>
#include <random>
#include <time.h>
struct commodity{
int index;
char  name[20];
double price;
double avgPrice;
double lastAvgPrice;
double past_4_readings[4];
int entered;
};
struct current_comm{
char  name[20];
double price;
};
struct only_names_buffer{
char  name[20];
double price;
int entered;
};
struct buffer{
int in_index;
int out_index; 
};
using namespace std;
int main(int argno, char ** arguments)
{	
/*********************************************************/
	// ftok to generate unique key
	int buffer_size=atoi(arguments[5]);
	key_t key = ftok("./",7);
	key_t key_buff = ftok("./",10);
	key_t key_buffer=ftok("./",11);
	// shmget returns an identifier in shmid
	int shmid = shmget(key,sizeof(struct commodity),0666|IPC_CREAT);
	int buffid = shmget(key_buff,sizeof(struct buffer),0666|IPC_CREAT);
	int shsegmentid = shmget(key_buffer,sizeof(struct only_names_buffer),0666|IPC_CREAT);
	int sem_id;
	double thisprice=0; 
	double v=0.0;
	std::default_random_engine generator;
        std::normal_distribution<float> distribution(atoi(arguments[2]),atoi(arguments[3]));
  	struct sembuf mutexWait[1] = {0, -1};//semaphore no ---- op to be done
	struct sembuf mutexSignal[1] = {0, 1};
	struct sembuf filledWait[1] = {1, -1};
	struct sembuf filledSignal[1] = {1, 1};
	struct sembuf emptyWait[1] = {2, -1};
	struct sembuf emptySignal[1] = {2, 1};
	struct current_comm my_comm;
	struct commodity *all_comm;
	struct buffer *mybufferinfo;
	struct only_names_buffer *buffer;
	all_comm = (struct commodity*)shmat(shmid,(void*)0,0);
	mybufferinfo = (struct buffer*)shmat(buffid,(void*)0,0);
	buffer=(struct only_names_buffer*)shmat(shsegmentid,(void*)0,0);
	sem_id= semget(key, 3, 0666 | IPC_CREAT);
	strcpy(my_comm.name,arguments[1]);
	strcpy(my_comm.name,arguments[1]);
/********************************************************/
	int c=1,j;
	while (true){
	thisprice = distribution(generator);
	struct timespec ts1;
    	timespec_get(&ts1, TIME_UTC);
   	char buff1[100];
   	strftime(buff1, sizeof buff1, "%D %T", gmtime(&ts1.tv_sec));
	fprintf(stdout,"[%s.%ld] %s: generating a new value of %.4f\n",buff1,ts1.tv_nsec,my_comm.name,thisprice);
	struct timespec ts2;
    	timespec_get(&ts2, TIME_UTC);
   	char buff2[100];
   	strftime(buff2, sizeof buff2, "%D %T", gmtime(&ts2.tv_sec));
	fprintf(stdout,"[%s.%ld] %s: trying to get mutex on shared buffer\n",buff2,ts2.tv_nsec,my_comm.name);
	semop(sem_id, emptyWait, 1); 
	semop(sem_id, mutexWait, 1); 
	struct timespec ts3;
    	timespec_get(&ts3, TIME_UTC);
   	char buff3[100];
   	strftime(buff3, sizeof buff3, "%D %T", gmtime(&ts3.tv_sec));
	fprintf(stdout,"[%s.%ld] %s: placing %.4f on shared buffer\n",buff3,ts3.tv_nsec,my_comm.name,thisprice);
	if(strcmp(buffer[(mybufferinfo->in_index)%buffer_size].name,"empty")==0){
			strcpy(buffer[(mybufferinfo->in_index)%buffer_size].name,my_comm.name);
			buffer[(mybufferinfo->in_index)%buffer_size].price=thisprice;
			if(c==1)
				buffer[(mybufferinfo->in_index)%buffer_size].entered=1;
			else 	
				buffer[(mybufferinfo->in_index)%buffer_size].entered=0;
	}
	c++;
	mybufferinfo->in_index++;
	//**************		
	semop(sem_id, mutexSignal, 1); 
	semop(sem_id, filledSignal, 1); 
	struct timespec ts4;
    	timespec_get(&ts4, TIME_UTC);
   	char buff4[100];
   	strftime(buff4, sizeof buff4, "%D %T", gmtime(&ts4.tv_sec));
	fprintf(stdout,"[%s.%ld] %s: sleeping for %d ms\n",buff4,ts4.tv_nsec,my_comm.name,atoi(arguments[4]));
	fprintf(stdout,"\n");
	usleep(1000*atoi(arguments[4]));
	
	}
/***********************************************************/
	return 0;
}

