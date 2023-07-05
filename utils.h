//
// Created by root on 6/12/23.
//

#ifndef KOLOS4_UTILS_H
#define KOLOS4_UTILS_H

#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define PORTION_SIZE 2
#define SHARED_STRUCT_NAME "temp11"
typedef struct {
    int totalSize;
    int arr[PORTION_SIZE];

    sem_t clientSem;
    sem_t serverSem;

    pthread_mutex_t serverBusy;
} shared_struct_t;

#endif //KOLOS4_UTILS_H
