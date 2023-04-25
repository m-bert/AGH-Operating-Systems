#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <wait.h>
#include <memory.h>
#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <semaphore.h>

#define HOME_PATH getenv("HOME")

#define HAIRDRESSER_SEM "/hairdresser_sem"
#define CHAIRS_SEM "/chairs_sem"
#define WAITING_SEM "/waiting_room_sem"

#define NO_SHARE 0
#define SHARE 1

#define HAIRDRESSER_SHM "/hairdresser_shm"
#define CHAIRS_SHM "/chairs_shm"
#define WAITING_SHM "/waiting_room_shm"

#define HAIRCUTS_NO 4

const int haircuts[] = {5, 7, 8, 10};

int get_haircut_time()
{
    return haircuts[rand() % HAIRCUTS_NO];
}

//============================================================================
// Array functions
//============================================================================

int find_index(int *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == -1)
        {
            return i;
        }
    }

    return -1;
}

void init_array(int *arr, int size)
{
    for (int i = 0; i < size; ++i)
    {
        arr[i] = -1;
    }
}

#endif