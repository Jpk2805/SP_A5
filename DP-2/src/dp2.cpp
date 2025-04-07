#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdlib.h>

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

	return 0;
}
