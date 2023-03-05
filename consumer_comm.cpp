#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "stdio.h"
#include <errno.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/sem.h>
#include<unistd.h>
#include <cstdint>
struct commodity{
int index;
char  name[20];
double price;
double avgPrice;
double lastAvgPrice;
double past_4_readings[4];
int entered;
};
struct buffer{
int in_index;
int out_index; 
};
struct only_names_buffer{
char  name[20];
double price;
int entered;
};
using namespace std;

int main(int argno, char ** arguments)
{
	int buffer_size;
	int i,v;
	int j;
	char consumed_name[20];
	int c;
	buffer_size=atoi(arguments[1]);
	// ftok to generate unique key
	key_t key = ftok("./",7);
	key_t key_buff = ftok("./",10);
	key_t key_buffer=ftok("./",11);
	// Semaphores initializations
	struct sembuf mutexWait[1] = {0, -1};//sem no ---- op to be done
	struct sembuf mutexSignal[1] = {0, 1};
	struct sembuf filledWait[1] = {1, -1};
	struct sembuf filledSignal[1] = {1, 1};
	struct sembuf emptyWait[1] = {2, -1};
	struct sembuf emptySignal[1] = {2, 1};
	int sem_id= semget(key, 3, 0666 | IPC_CREAT);
	semctl (sem_id, 0, SETVAL, 1);//0-->mutex 
	semctl (sem_id, 1, SETVAL, 0);//1-->number of filled spaces in all_comm array 
	semctl (sem_id, 2, SETVAL, buffer_size);//2-->number of empty spaces in all_comm array
	/***********************************************************/
	// shmget returns an identifier in shmid
	int shmid = shmget(key,sizeof(struct commodity),0666|IPC_CREAT);
	int buffid = shmget(key_buff,sizeof(struct buffer),0666|IPC_CREAT);
	int shsegmentid = shmget(key_buffer,sizeof(struct only_names_buffer),0666|IPC_CREAT);
	// INITIALIZE ARRAY OF ALL COMMODITIES 
	struct commodity *all_comm;
	struct buffer *mybufferinfo;
	struct only_names_buffer *buffer;
	all_comm = (struct commodity*)shmat(shmid,(void*)0,0);
	mybufferinfo = (struct buffer*)shmat(buffid,(void*)0,0);
	buffer=(struct only_names_buffer*)shmat(shsegmentid,(void*)0,0);
	mybufferinfo->in_index=0; 
	mybufferinfo->out_index=0;
	double thisprice=0.0;
	strcpy(all_comm[0].name,"ALUMINIUM");strcpy(all_comm[1].name,"COPPER");
 	strcpy(all_comm[2].name,"COTTON");strcpy(all_comm[3].name,"CRUDEOIL");
 	strcpy(all_comm[4].name,"GOLD");strcpy(all_comm[5].name,"LEAD");
	strcpy(all_comm[6].name,"MENTHAOIL");strcpy(all_comm[7].name,"NATURALGAS");
	strcpy(all_comm[8].name,"NICKEL");strcpy(all_comm[9].name,"SILVER");strcpy(all_comm[10].name,"ZINC");
	for(i=0;i<11;i++){
		all_comm[i].index=i;
		all_comm[i].price=0.00;
		all_comm[i].avgPrice=0.00;
		all_comm[i].lastAvgPrice=0.00;
		for(int j=0;j<4;j++){
			all_comm[i].past_4_readings[j]=0.00;
		}
	}
	for(int p=0;p<buffer_size;p++){
		strcpy(buffer[p].name,"empty");
		buffer[p].price=0.0;
	}
	//first print
	printf("\e[1;1H\e[2J");
	printf("+");for(i=0;i<37;i++) printf("-");printf("+\n");
	printf("| Currency      |  Price   | AvgPrice |\n");
	printf("+");for(i=0;i<37;i++) printf("-");printf("+\n");
	printf("| %s     | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[0].name,all_comm[0].price,all_comm[0].avgPrice);
	printf("| %s        | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[1].name,all_comm[1].price,all_comm[1].avgPrice);
	printf("| %s        | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[2].name,all_comm[2].price,all_comm[2].avgPrice);
	printf("| %s      | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[3].name,all_comm[3].price,all_comm[3].avgPrice);
	printf("| %s          | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[4].name,all_comm[4].price,all_comm[4].avgPrice);
	printf("| %s          | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[5].name,all_comm[5].price,all_comm[5].avgPrice);
	printf("| %s     | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[6].name,all_comm[6].price,all_comm[6].avgPrice);
	printf("| %s    | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[7].name,all_comm[7].price,all_comm[7].avgPrice);
	printf("| %s        | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[8].name,all_comm[8].price,all_comm[8].avgPrice);
	printf("| %s        | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[9].name,all_comm[9].price,all_comm[9].avgPrice);
	printf("| %s          | \033[;36m%7.2lf\033[0m  | \033[;36m%7.2lf\033[0m  |\n",all_comm[10].name,all_comm[10].price,all_comm[10].avgPrice);
	printf("+");for(i=0;i<37;i++) printf("-");printf("+\n");
	while (true){
	semop(sem_id, filledWait, 1); 
	semop(sem_id, mutexWait, 1); 
	// CONSUME
	strcpy(consumed_name,buffer[(mybufferinfo->out_index)%buffer_size].name);
	thisprice=buffer[(mybufferinfo->out_index)%buffer_size].price;
	v=0.0;
	//**************
	for(i=0;i<11;i++){
		if (strcmp(all_comm[i].name,consumed_name)==0){
			all_comm[i].price=thisprice;
			if(buffer[(mybufferinfo->out_index)%buffer_size].entered==1){
				all_comm[i].entered=buffer[(mybufferinfo->out_index)%buffer_size].entered;
				buffer[(mybufferinfo->out_index)%buffer_size].entered=0;
				}
			if(all_comm[i].entered==1){
				all_comm[i].lastAvgPrice=0.0;
				all_comm[i].avgPrice=thisprice;
			}
			else if(all_comm[i].entered==2){
				all_comm[i].lastAvgPrice=all_comm[i].avgPrice;
				all_comm[i].avgPrice=(thisprice+all_comm[i].past_4_readings[0])/2.00;
			}
			else if(all_comm[i].entered==3){
				all_comm[i].lastAvgPrice=all_comm[i].avgPrice;
				all_comm[i].avgPrice=(thisprice+all_comm[i].past_4_readings[0]+all_comm[i].past_4_readings[1])/3.00;
			}
			else if(all_comm[i].entered==4){
				all_comm[i].lastAvgPrice=all_comm[i].avgPrice;
				all_comm[i].avgPrice=(thisprice+all_comm[i].past_4_readings[0]+all_comm[i].past_4_readings[1]+all_comm[i].past_4_readings[2])/4.00;
			}
			
		}
	}	
	for(i=0;i<11;i++){
		if (strcmp(all_comm[i].name,consumed_name)==0){
			c=i;
			break;
		}
	}
	printf("\033[%d;1H",c+4);
	if(all_comm[c].price>=all_comm[c].past_4_readings[3] &&	all_comm[c].avgPrice>=all_comm[c].lastAvgPrice){
	if(all_comm[c].index==0)
		printf("| %s     | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[0].name,all_comm[0].price,all_comm[0].avgPrice);
	if(all_comm[c].index==1)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[1].name,all_comm[1].price,all_comm[1].avgPrice);
	if(all_comm[c].index==2)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[2].name,all_comm[2].price,all_comm[2].avgPrice);
	if(all_comm[c].index==3)
		printf("| %s      | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[3].name,all_comm[3].price,all_comm[3].avgPrice);			
	if(all_comm[c].index==4)
		printf("| %s          | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[4].name,all_comm[4].price,all_comm[4].avgPrice);
	if(all_comm[c].index==5)
		printf("| %s          | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[5].name,all_comm[5].price,all_comm[5].avgPrice);
	if(all_comm[c].index==6)
		printf("| %s     | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[6].name,all_comm[6].price,all_comm[6].avgPrice);
	if(all_comm[c].index==7)
		printf("| %s    | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[7].name,all_comm[7].price,all_comm[7].avgPrice);
	if(all_comm[c].index==8)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[8].name,all_comm[8].price,all_comm[8].avgPrice);	
	if(all_comm[c].index==9)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[9].name,all_comm[9].price,all_comm[9].avgPrice);
	if(all_comm[c].index==10)
		printf("| %s          | \033[;32m%7.2lf↑\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[10].name,all_comm[10].price,all_comm[10].avgPrice);			
	}
	else if(all_comm[c].price<=all_comm[c].past_4_readings[3] && all_comm[c].avgPrice<=all_comm[c].lastAvgPrice){
	if(all_comm[c].index==0)
		printf("| %s     | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[0].name,all_comm[0].price,all_comm[0].avgPrice);
	if(all_comm[c].index==1)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[1].name,all_comm[1].price,all_comm[1].avgPrice);
	if(all_comm[c].index==2)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[2].name,all_comm[2].price,all_comm[2].avgPrice);
	if(all_comm[c].index==3)
		printf("| %s      | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[3].name,all_comm[3].price,all_comm[3].avgPrice);			
	if(all_comm[c].index==4)
		printf("| %s          | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[4].name,all_comm[4].price,all_comm[4].avgPrice);
	if(all_comm[c].index==5)
		printf("| %s          | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[5].name,all_comm[5].price,all_comm[5].avgPrice);
	if(all_comm[c].index==6)
		printf("| %s     | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[6].name,all_comm[6].price,all_comm[6].avgPrice);
	if(all_comm[c].index==7)
		printf("| %s    | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[7].name,all_comm[7].price,all_comm[7].avgPrice);
	if(all_comm[c].index==8)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[8].name,all_comm[8].price,all_comm[8].avgPrice);	
	if(all_comm[c].index==9)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[9].name,all_comm[9].price,all_comm[9].avgPrice);
	if(all_comm[c].index==10)
		printf("| %s          | \033[;31m%7.2lf↓\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[10].name,all_comm[10].price,all_comm[10].avgPrice);			
	}
	else if(all_comm[c].price<=all_comm[c].past_4_readings[3] &&	all_comm[c].avgPrice>=all_comm[c].lastAvgPrice){
	if(all_comm[c].index==0)
		printf("| %s     | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[0].name,all_comm[0].price,all_comm[0].avgPrice);
	if(all_comm[c].index==1)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[1].name,all_comm[1].price,all_comm[1].avgPrice);
	if(all_comm[c].index==2)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[2].name,all_comm[2].price,all_comm[2].avgPrice);
	if(all_comm[c].index==3)
		printf("| %s      | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[3].name,all_comm[3].price,all_comm[3].avgPrice);			
	if(all_comm[c].index==4)
		printf("| %s          | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[4].name,all_comm[4].price,all_comm[4].avgPrice);
	if(all_comm[c].index==5)
		printf("| %s          | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[5].name,all_comm[5].price,all_comm[5].avgPrice);
	if(all_comm[c].index==6)
		printf("| %s     | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[6].name,all_comm[6].price,all_comm[6].avgPrice);
	if(all_comm[c].index==7)
		printf("| %s    | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[7].name,all_comm[7].price,all_comm[7].avgPrice);
	if(all_comm[c].index==8)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[8].name,all_comm[8].price,all_comm[8].avgPrice);	
	if(all_comm[c].index==9)
		printf("| %s        | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[9].name,all_comm[9].price,all_comm[9].avgPrice);
	if(all_comm[c].index==10)
		printf("| %s          | \033[;31m%7.2lf↓\033[0m | \033[;32m%7.2lf↑\033[0m |\n",all_comm[10].name,all_comm[10].price,all_comm[10].avgPrice);			
	}
	else if(all_comm[c].price>=all_comm[c].past_4_readings[3] &&	all_comm[c].avgPrice<=all_comm[c].lastAvgPrice){
	if(all_comm[c].index==0)
		printf("| %s     | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[0].name,all_comm[0].price,all_comm[0].avgPrice);
	if(all_comm[c].index==1)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[1].name,all_comm[1].price,all_comm[1].avgPrice);
	if(all_comm[c].index==2)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[2].name,all_comm[2].price,all_comm[2].avgPrice);
	if(all_comm[c].index==3)
		printf("| %s      | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[3].name,all_comm[3].price,all_comm[3].avgPrice);			
	if(all_comm[c].index==4)
		printf("| %s          | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[4].name,all_comm[4].price,all_comm[4].avgPrice);
	if(all_comm[c].index==5)
		printf("| %s          | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[5].name,all_comm[5].price,all_comm[5].avgPrice);
	if(all_comm[c].index==6)
		printf("| %s     | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[6].name,all_comm[6].price,all_comm[6].avgPrice);
	if(all_comm[c].index==7)
		printf("| %s    | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[7].name,all_comm[7].price,all_comm[7].avgPrice);
	if(all_comm[c].index==8)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[8].name,all_comm[8].price,all_comm[8].avgPrice);	
	if(all_comm[c].index==9)
		printf("| %s        | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[9].name,all_comm[9].price,all_comm[9].avgPrice);
	if(all_comm[c].index==10)
		printf("| %s          | \033[;32m%7.2lf↑\033[0m | \033[;31m%7.2lf↓\033[0m |\n",all_comm[10].name,all_comm[10].price,all_comm[10].avgPrice);			
	}
	printf("\033[15;1H");
	printf("\n");
	for(i=0;i<11;i++){
		if (strcmp(all_comm[i].name,consumed_name)==0){
			if(all_comm[i].entered==1){
				all_comm[i].past_4_readings[0]=thisprice;
				all_comm[i].past_4_readings[3]=thisprice;
			}
			else if(all_comm[i].entered==2){
				all_comm[i].past_4_readings[1]=thisprice;
				all_comm[i].past_4_readings[3]=thisprice;
			}
			else if(all_comm[i].entered==3){
				all_comm[i].past_4_readings[2]=thisprice;
				all_comm[i].past_4_readings[3]=thisprice;
			}
			else if(all_comm[i].entered==4){
				all_comm[i].past_4_readings[3]=thisprice;
			}
			if(all_comm[i].entered>4){
				for(j=0;j<4;j++){
					v+=all_comm[i].past_4_readings[j];
				}
				v=v+thisprice;
				all_comm[i].lastAvgPrice=all_comm[i].avgPrice;
				all_comm[i].avgPrice=v/5.00;
				for(j=0;j<3;j++){
					all_comm[i].past_4_readings[j]=all_comm[i].past_4_readings[j+1];
				}
				all_comm[i].past_4_readings[3]=all_comm[i].price;
			}
			all_comm[i].entered++;
		}
	}
	strcpy(buffer[(mybufferinfo->out_index)%buffer_size].name,"empty");
	buffer[(mybufferinfo->out_index)%buffer_size].price=0.0;
	mybufferinfo->out_index++;
	semop(sem_id, mutexSignal, 1); 
	semop(sem_id, emptySignal, 1); 
	}
	
	return 0;
	
}

