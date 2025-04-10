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

void sigint_handler(int sig);
void alarm_handler(int sig);
void print_histogram(void);


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

	struct itimerval tv;
    tv.it_value.tv_sec = 2;
    tv.it_value.tv_usec = 0;
    tv.it_interval.tv_sec = 2;
    tv.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &tv, NULL);

    time_t last_print = time(NULL);
	while (1){
		pause();
        if (done && shm->readIndex == shm->writeIndex) {
            print_histogram();
            shmdt(shm);
            shmctl(shmId, IPC_RMID, NULL);
            semctl(semId, 0, IPC_RMID);
            puts("Shazam !!");
            exit(0);
        }
        if (time(NULL) - last_print >= 10) {
            print_histogram();
            last_print = time(NULL);
        }
	}

	
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

void print_histogram(void) {
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