#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>

int main(void){
	//dp-1 will run dp-2 
	//dp-2 will access the shared memory created by dp-1 
	//by using shmid passed by dp-1
	//
	//then in while loop it will write the character on the circuler buffer
	//sleep to 1/20 th of second
	return 0;
}
