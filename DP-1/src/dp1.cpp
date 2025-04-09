// File: dp1.c
// Project: A-05: The Histogram System
// Programmers: Vishvesh Lakhani, Ayush Rakholiya, Jay Patel, Hetvi Kaswala
// Description: This program acts as the first Data Producer (DP-1) in the HISTO-SYSTEM.
//     			It creates and initializes shared memory and semaphores, then forks and 
//     			launches the second Data Producer (DP-2). DP-1 continuously generates a 
//     			batch of random characters and writes them into the shared memory buffer,
//     				ensuring synchronization using semaphores.
// Date: April 7, 2025
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include<cstring>
#include<errno.h>
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

int main(void) {
	
	//initialize the empty shared memory.
	key_t shm_key = ftok(SHM_KEY_PATH, SHM_KEY_ID);
	key_t sem_key = ftok(SEM_KEY_PATH, SEM_KEY_ID);

	shmId = shmget(shm_key, sizeof(shmRegion), IPC_CREAT | 0660);
	if (shmId < 0) { perror("shmget"); exit(1); }
	shm = (shmRegion *)shmat(shmId, NULL, 0);

	//initilizing both the write index and read index to zero.
	shm->readIndex = shm->writeIndex = 0;
	memset(shm->buffer, 0, BUF_SIZE);
	semId = semget(sem_key, 1, IPC_CREAT | 0660);
	if (semId < 0) { perror("semget"); exit(1); }
	semctl(semId, 0, SETVAL, 1);

	signal(SIGINT, cleanup);
	
	//fork the dp-2 then run it passing the pid of this process.
	pid_t pid = fork();
	if (pid == 0) {
		char arg1[16], arg2[16];
		snprintf(arg1, sizeof(arg1), "%d", shmId);
		snprintf(arg2, sizeof(arg2), "%d", semId);
		execlp("../../DP-2/bin/DP-2", "dp2", arg1, arg2, NULL);
		perror("execlp dp2");
		exit(1);
	}

	// assign the random.
	srand(time(NULL) ^ getpid());
	//create a while loop for generating letters.
    while (1) {
        semop(semId, &semAcquire, 1);
        int space;
        if (shm->writeIndex >= shm->readIndex)
            space = BUF_SIZE - shm->writeIndex + shm->readIndex - 1;
        else
            space = shm->readIndex - shm->writeIndex - 1;
		
        int to_write = (space >= 20 ? 20 : space);
        for (int i = 0; i < to_write; ++i) {
            shm->buffer[shm->writeIndex++] = 'A' + (rand() % 20);
            if (shm->writeIndex >= BUF_SIZE) shm->writeIndex = 0;
        }
        semop(semId, &semRelease, 1);

        sleep(2);
    }
 	return 0;
}