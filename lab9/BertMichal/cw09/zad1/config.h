#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>

#define GIFTS_TO_DELIVER 3

#define REINDEERS 9
#define ELVES 5

#define MAX_WATING_ELVES 3

#define VACATION_TIME_MIN 5
#define VACATION_TIME_MAX 10

#define DELIVERY_TIME_MIN 2
#define DELIVERY_TIME_MAX 4

#define WORK_TIME_MIN 2
#define WORK_TIME_MAX 5

#define SOLVE_TIME_MIN 1
#define SOLVE_TIME_MAX 2

int get_random_int(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

void go_on_vacation()
{
    sleep(get_random_int(VACATION_TIME_MIN, VACATION_TIME_MAX));
}

void deliver_gifts()
{
    sleep(get_random_int(DELIVERY_TIME_MIN, DELIVERY_TIME_MAX));
}

void work()
{
    sleep(get_random_int(WORK_TIME_MIN, WORK_TIME_MAX));
}

void solve_problems()
{
    sleep(get_random_int(SOLVE_TIME_MIN, SOLVE_TIME_MAX));
}