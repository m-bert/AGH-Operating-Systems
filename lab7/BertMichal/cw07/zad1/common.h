#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <signal.h>
#include <wait.h>
#include <memory.h>
#include <errno.h>
#include <time.h>

#define HOME_PATH getenv("HOME")

#define HAIRDRESSERS_SEED 1
#define CHAIRS_SEED 2
#define WAITING_SEED 3
#define SEMAPHORES_SEED 4

#define HAIRDRESSERS_KEY ftok(HOME_PATH, HAIRDRESSERS_SEED)
#define CHAIRS_KEY ftok(HOME_PATH, CHAIRS_SEED)
#define WAITING_KEY ftok(HOME_PATH, WAITING_SEED)

#define SEMAPHORES_KEY ftok(HOME_PATH, SEMAPHORES_SEED)
#define WAIT 0
#define NO_WAIT IPC_NOWAIT
#define SEM_NO 3
#define HAIRDRESSER_SEM 0
#define CHAIRS_SEM 1
#define WAITING_SEM 2

#define HAIRCUTS_NO 4

const int haircuts[] = {2, 3, 5, 10};

int get_haircut_time()
{
    return haircuts[rand() % HAIRCUTS_NO];
}

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

void increment_sem(int sem_id, int sem_num)
{
    struct sembuf buffer[1];

    buffer[0].sem_num = sem_num;
    buffer[0].sem_op = 1;
    buffer[0].sem_flg = 0;

    semop(sem_id, buffer, 1);
}

void decrement_sem(int sem_id, int sem_num, int flag)
{
    struct sembuf buffer[1];

    buffer[0].sem_num = sem_num;
    buffer[0].sem_op = -1;
    buffer[0].sem_flg = flag;

    semop(sem_id, buffer, 1);
}

int get_sem_value(int sem_id, int sem_num)
{
    return semctl(sem_id, sem_num, GETVAL);
}

void set_sem_value(int sem_id, int sem_num, int value)
{
    struct sembuf buffer[1];

    buffer[0].sem_num = sem_num;
    buffer[0].sem_op = value;
    buffer[0].sem_flg = 0;

    semop(sem_id, buffer, 1);
}

#endif