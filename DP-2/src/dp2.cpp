#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<signal.h>
#include"../inc/common.h"

int semId, shmId;
shmRegion *shm = NULL;

void cleanup(int sig) {
    if (shm) shmdt(shm);
    exit(0);
}

int main(void){

	if (argc != 3) {
        fprintf(stderr, "Usage: dp2 <shmId> <semId>\n");
        return 1;
    }
    shmId = atoi(argv[1]);
    semId = atoi(argv[2]);

    signal(SIGINT, cleanup);

	// dp2 forked // semphore arguments
	// may be something is missing
	pid_t dp2_pid = getpid(), dp1_pid = getppid();
    char args[4][16];
    snprintf(args[0], 16, "%d", shmId);
    snprintf(args[1], 16, "%d", semId);
    snprintf(args[2], 16, "%d", dp1_pid);
    snprintf(args[3], 16, "%d", dp2_pid);
    if (fork() == 0) {
        execlp("../../DC/bin/DC", "dc", args[0], args[1], args[2], args[3], NULL);
        perror("execlp dc");
        exit(1);
    }

	shm = (shmRegion*)shmat(shmId, NULL, 0);
    if (shm == (void *) -1) {
        perror("shmat failed");
        exit(1);
    }

    srand(time(NULL) ^ dp2_pid);

    while (1) {
        semop(semId, &semAcquire, 1);

        int next = (shm->writeIndex + 1) % BUF_SIZE;
        if (next != shm->readIndex) {
            shm->buffer[shm->writeIndex] = 'A' + (rand() % 20);
            shm->writeIndex = next;
        }

        semop(semId, &semRelease, 1);
        usleep(50000);
    }

	return 0;
}
