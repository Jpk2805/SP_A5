#include <semaphore.h>

#define BUFFER_SIZE 256

typedef struct {
    sem_t mutex;
    char buffer[BUFFER_SIZE];
    int read_idx;
    int write_idx;
} ShmData;

// testing the branch commits
