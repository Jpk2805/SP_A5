#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include "../inc/common.h"

int shmId, semId;
int dp1PID, dp2PID;
shmRegion *shm = NULL;
int counts[20] = {0}; // will keep track of the number of latters 
volatile sig_atomic_t done = 0;

void sigint_handler(int sig);
void alarm_handler(int sig);


int main(int argc, char* argv[]){

	if (argc != 5) {
        fprintf(stderr, "Usage: dc <shm_id> <sem_id> <dp1_pid> <dp2_pid>\n");
        return 1;
    }
    shmId = atoi(argv[1]);
    semId = atoi(argv[2]);
    dp1PID = atoi(argv[3]);
    dp2PID = atoi(argv[4]);

    shm = (shmRegion*)shmat(shmId, NULL, 0);

    signal(SIGINT, sigint_handler);
    signal(SIGALRM, alarm_handler);

	//while loop for reading from the buffer 
	//
	//crear the screen then print the count of each latter.
	return 0;
}


void sigint_handler(int sig) {
    done = 1;
    kill(dp1PID, SIGINT);
    kill(dp2PID, SIGINT);
}

void alarm_handler(int sig) {

    semop(semId, &semAcquire, 1);
    int to_read = 60;
    for (int i = 0; i < to_read; ++i) {
        if (shm->readIndex == shm->writeIndex) break;
        char c = shm->buffer[shm->readIndex++];
        if (shm->readIndex >= BUF_SIZE) shm->readIndex = 0;
        if (c >= 'A' && c <= 'T') counts[c - 'A']++;
    }
    semop(semId, &semRelease, 1);
}