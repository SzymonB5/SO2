//
// Created by root on 6/12/23.
//

#include "client.h"
#include "utils.h"

int main(int argc, char **argv) {
    int count = 10; // atoi(*(argv + 1));

    int fd = shm_open(SHARED_STRUCT_NAME, O_RDWR, 0666);
    if (fd == -1)
        return puts("Failed to connect to the server"), 1;

    shared_struct_t *sharedStruct = mmap(NULL, sizeof(shared_struct_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    int arr[count];
    pthread_mutex_lock(&sharedStruct->serverBusy);
    sharedStruct->totalSize = count;
    int iterations = count / PORTION_SIZE + 1;

    sem_post(&sharedStruct->serverSem);
    sem_wait(&sharedStruct->clientSem);

    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < PORTION_SIZE; j++)
            arr[i * PORTION_SIZE + j] = sharedStruct->arr[j];

        sem_post(&sharedStruct->serverSem);
        sem_wait(&sharedStruct->clientSem);
    }

    pthread_mutex_unlock(&sharedStruct->serverBusy);

    for (int i = 0; i < count; i++)
        printf("%d ", arr[i]);

    putchar(10);

    munmap(sharedStruct, sizeof(shared_struct_t));
    close(fd);

    return 0;
}
