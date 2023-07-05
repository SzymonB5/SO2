//
// Created by root on 6/12/23.
//

#include "server.h"
#include "utils.h"

int fd;
shared_struct_t *sharedStruct;
_Atomic volatile int tasksPerformed = 0;

pthread_t p1, p2, p3;
pthread_mutex_t serverBusy;

void createServer(void) {
    fd = shm_open(SHARED_STRUCT_NAME, O_CREAT | O_RDWR | O_EXCL, 0666);
    ftruncate(fd, sizeof(shared_struct_t));
    sharedStruct = mmap(NULL, sizeof(shared_struct_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    sem_init(&sharedStruct->serverSem, 1, 0);
    sem_init(&sharedStruct->clientSem, 1, 0);

    pthread_mutexattr_t mutexattr;
    pthread_mutexattr_init(&mutexattr);
    pthread_mutexattr_setpshared(&mutexattr, 1);
    pthread_mutex_init(&sharedStruct->serverBusy, &mutexattr);
    pthread_mutexattr_destroy(&mutexattr);

    pthread_mutex_init(&serverBusy, &mutexattr);
}

_Noreturn void *displayStatistics(void *arg) {
    while (1) {
        sleep(2u);
        printf("Tasks performed: %d\n", tasksPerformed);
    }
}

_Noreturn void *waitForCommands(void *arg) {
    char command[5];
    inf_loop:

    scanf("%s", command);

    printf("");
    if (strcmp(command, "quit") == 0) {
        stopThreads();
    }
    else if (strcmp(command, "stat") == 0) {
        printf("Currently: %d\n", tasksPerformed);
    }
    else {
        puts("Unknown command");
    }

    fflush(stdin);

    goto inf_loop;
}

_Noreturn void *routine(void *arg) {

    while (1) {
        sem_wait(&sharedStruct->serverSem);

        pthread_mutex_lock(&serverBusy);

        int iterations = sharedStruct->totalSize / PORTION_SIZE + 1;
        int tempArr[sharedStruct->totalSize];

        for (int i = 0; i < sharedStruct->totalSize; i++)
            tempArr[i] = rand() % 100;

        for (int i = 0; i < iterations; i++) {
            for (int j = 0; j < PORTION_SIZE; j++)
                if (i * PORTION_SIZE + j < sharedStruct->totalSize)
                    sharedStruct->arr[j] = tempArr[i * PORTION_SIZE + j];

            sem_post(&sharedStruct->clientSem);
            sem_wait(&sharedStruct->serverSem);
        }
        sem_post(&sharedStruct->clientSem);

        pthread_mutex_unlock(&serverBusy);

        tasksPerformed++;
    }

}

void stopThreads() {
    pthread_mutex_lock(&serverBusy);
    pthread_cancel(p1);
    pthread_mutex_unlock(&serverBusy);

    pthread_cancel(p2);
    pthread_cancel(p3);
}

int main(void) {
    srand(time(NULL));
    createServer();

    pthread_create(&p1, NULL, routine, NULL);
    pthread_create(&p2, NULL, displayStatistics, NULL);
    pthread_create(&p3, NULL, waitForCommands, NULL);

    pthread_join(p3, NULL);

    pthread_mutex_destroy(&sharedStruct->serverBusy);
    pthread_mutex_destroy(&serverBusy);
    sem_destroy(&sharedStruct->serverSem);
    sem_destroy(&sharedStruct->clientSem);

    munmap(sharedStruct, sizeof(shared_struct_t));
    close(fd);
    shm_unlink(SHARED_STRUCT_NAME);

    return 0;
}
