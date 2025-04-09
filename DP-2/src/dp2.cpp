// File : dp2.c
// Project: A-05: The Histogram System
// Programmers: Vishvesh Lakhani, Ayush Rakholiya, Jay Patel, Hetvi Kaswala
// Description: This program is the second Data Producer (DP-2) in the HISTO-SYSTEM. 
//              It attaches to a shared memory segment and uses semaphores to synchronize 
//              access with other processes. DP-2 continuously generates random characters 
//              and writes them to the shared buffer if space is available. It also forks 
//              the Data Consumer (DC) process and passes required IDs via command-line arguments.
// Date: April 7, 2025

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

// Function: cleanup
// Description: Handles cleanup operations when a termination signalis received.
//              It detaches the shared memory segment and exits the program gracefully.
// Parameters: 
//      int sig - The signal number that triggered the handler.
// Returns: void
void cleanup(int sig) {
    if (shm) shmdt(shm);
    exit(0);
}

int main(int argc, char* argv[]){

	if (argc != 3) {
        fprintf(stderr, "Usage: dp2 <shmId> <semId>\n");
        return 1;
    }

    // Convert input arguments to integers
    shmId = atoi(argv[1]);
    semId = atoi(argv[2]);

    signal(SIGINT, cleanup);

    // Prepare arguments to pass to the DC (data consumer) program
	// dp2 forked // semphore arguments
	// May be something is missing
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

    // Seed the random number generator using time and dp2 process ID
    srand(time(NULL) ^ dp2_pid);

    // Main loop: continuously write random characters to shared memory
    while (1) {
        semop(semId, &semAcquire, 1);

        int next = (shm->writeIndex + 1) % BUF_SIZE;
        if (next != shm->readIndex) {
            shm->buffer[shm->writeIndex] = 'A' + (rand() % 20);
            shm->writeIndex = next;
        }
        // Release the semaphore after writtten
        semop(semId, &semRelease, 1);
        usleep(50000);
    }

	return 0;
}
