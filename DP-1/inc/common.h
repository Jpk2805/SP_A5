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
    int read_idx;
    int write_idx;
    char buffer[BUF_SIZE];
} shm_region;

static struct sembuf sem_acquire = { 0, -1, SEM_UNDO };
static struct sembuf sem_release = { 0, +1, SEM_UNDO };