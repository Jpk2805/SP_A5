// File: dp1.c
// Project: A-05: The Histogram System
// Programmers: Vishvesh Lakhani, Ayush Rakholiya, Jay Patel, Hetvi Kaswala
// Description: This program is Data cosnsumer which will get started by the DP-2
//          and after it joins to the shared memory it will read through the circuler buffer
//          by using the readIndex specified in the common.h file.
// Date: April 7, 2025

#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<time.h>
#include<signal.h>
#include"../inc/common.h"

int shmId, semId;
int dp1PID, dp2PID;
shmRegion *shm = NULL;
int counts[20] = {0}; // will keep track of the number of latters 
volatile sig_atomic_t done = 0;

void sigintHandler(int sig);
void alarmHandler(int sig);
void printHistogram(void);


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

    signal(SIGINT, sigintHandler);
    signal(SIGALRM, alarmHandler);
    
    struct itimerval tv;
    tv.it_value.tv_sec = 2;
    tv.it_value.tv_usec = 0;
    tv.it_interval.tv_sec = 2;
    tv.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &tv, NULL);

    time_t lastPrint = time(NULL);
	while (1){
		pause();
        if (done && shm->readIndex == shm->writeIndex) {
            printHistogram();
            shmdt(shm);
            shmctl(shmId, IPC_RMID, NULL);
            semctl(semId, 0, IPC_RMID);
            puts("Shazam !!");
            exit(0);
        }
        if (time(NULL) - lastPrint >= 10) {
            printHistogram();
            lastPrint = time(NULL);
        }
	}

	
	return 0;
}

// Function: sigintHandler
// Description: after reciving the SIGINT signal this function will kill the dp-1 and dp-2
// Parameters: 
//      int sig - The signal number that triggered the handler.
// Returns: void

void sigintHandler(int sig) {
    done = 1;
    kill(dp1PID, SIGINT); 
    kill(dp2PID, SIGINT);
}

// Function: alarmHandler
// Description: after reciving the alarm signal this function will read from the buffer
//              60 character at a time also it will lock the shared memory using semaphore. 
// Parameters: 
//      int sig - The signal number that triggered the handler.
// Returns: void

void alarmHandler(int sig) {

    semop(semId, &semAcquire, 1);
    int toRead = 60;
    for (int i = 0; i < toRead; ++i) {
        if (shm->readIndex == shm->writeIndex) break;
        char c = shm->buffer[shm->readIndex++];
        if (shm->readIndex >= BUF_SIZE) shm->readIndex = 0;
        if (c >= 'A' && c <= 'T') counts[c - 'A']++;
    }
    semop(semId, &semRelease, 1);
}

// Function: printHistogram
// Description: this function will print the histogram when called 
// Parameters: 
//      void
// Returns: void

void printHistogram(void) {
    system("clear");
    for (int i = 0; i < 20; ++i) {
        int n = counts[i];
        printf("%c-%03d ", 'A'+i, n);
        int hundreds = n/100, tens = (n%100)/10, ones = n%10;
        for(int j=0;j<hundreds;j++) putchar('*');
        for(int j=0;j<tens;j++) putchar('+');
        for(int j=0;j<ones;j++) putchar('-');
        putchar('\n');
    }
}