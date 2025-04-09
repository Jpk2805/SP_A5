// File: common.h
// Project: A-05: The Histogram System
// Programmers: Vishvesh Lakhani, Ayush Rakholiya, Jay Patel, Hetvi Kaswala
// Description: This header file contains shared constants, structure definitions, and semaphore 
//               operations used across all components of the HISTO-SYSTEM project, including 
//               Data Producers and the Data Consumer. It defines the shared memory region structure, 
//               suffer size, IPC keys, and semaphore operations for synchronization.
// Date: April 7, 2025
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <signal.h>

#define BUF_SIZE    256
#define SHM_KEY_PATH "/tmp"
#define SHM_KEY_ID   42
#define SEM_KEY_PATH "/tmp"
#define SEM_KEY_ID   24 

typedef struct {
    int readIndex;
    int writeIndex;
    char buffer[BUF_SIZE];
} shmRegion;

static struct sembuf semAcquire = { 0, -1, SEM_UNDO };
static struct sembuf semRelease = { 0, +1, SEM_UNDO };
