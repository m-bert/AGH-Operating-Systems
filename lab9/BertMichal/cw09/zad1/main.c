#include "config.h"

int waiting_reindeers = 0;
int waiting_elves = 0;
int delivered_gifts = 0;

pthread_mutex_t event_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t event = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elves_cond = PTHREAD_COND_INITIALIZER;

void trigger_event();

void *santa_routine();
void *elf_routine();
void *reindeer_routine();

int main(int argc, char *argv[])
{
    srand(1234);

    pthread_t santa_thread;
    pthread_create(&santa_thread, NULL, santa_routine, NULL);

    pthread_t *reindeers_threads = calloc(REINDEERS, sizeof(pthread_t));
    for (int i = 0; i < REINDEERS; ++i)
    {
        pthread_create(&reindeers_threads[i], NULL, reindeer_routine, NULL);
    }

    while (delivered_gifts < GIFTS_TO_DELIVER)
    {
    }

    free(reindeers_threads);

    return 0;
}

void trigger_event()
{
    pthread_mutex_lock(&event_mutex);
    pthread_cond_broadcast(&event);
    pthread_mutex_unlock(&event_mutex);
}

void *santa_routine()
{
    while (delivered_gifts < GIFTS_TO_DELIVER)
    {
        pthread_cond_wait(&event, &event_mutex);

        if (waiting_elves == 3)
        {
            printf("Mikołaj: Budzę się\n");
            solve_problems();
            waiting_elves = 0;
        }

        if (waiting_reindeers == 9)
        {
            printf("Mikołaj: Budzę się\n");
            printf("Mikołaj: Dostarczam zabawki\n");
            deliver_gifts();

            pthread_mutex_lock(&reindeer_mutex);

            waiting_reindeers = 0;
            pthread_cond_broadcast(&reindeer_cond);

            pthread_mutex_unlock(&reindeer_mutex);

            ++delivered_gifts;
        }
    }

    return NULL;
}

void *elf_routine()
{
    while (deliver_gifts < GIFTS_TO_DELIVER)
    {
        work();

        pthread_mutex_lock(&elves_mutex);

        if (waiting_elves == MAX_WATING_ELVES)
        {
            printf("Elf: Samodzielnie rozwiązuję swój problem [%d]", gettid());
            pthread_mutex_unlock(&elves_mutex);

            continue;
        }

        if (waiting_elves == MAX_WATING_ELVES - 1)
        {
            printf("Elf: Wybudzam Mikołaja [%d]", gettid());

            while (waiting_elves > 0)
            {
                pthread_cond_wait(&elves_cond);
            }
        }
    }
}

void *reindeer_routine()
{
    while (delivered_gifts < GIFTS_TO_DELIVER)
    {
        go_on_vacation();

        pthread_mutex_lock(&reindeer_mutex);
        if (waiting_reindeers == REINDEERS - 1)
        {
            printf("Renifer: Wybudzam Mikołaja [%d]\n", gettid());
        }
        else
        {
            printf("Renifer: Czeka %d reniferów na Mikołaja [%d]\n", waiting_reindeers, gettid());
        }

        ++waiting_reindeers;

        trigger_event();

        while (waiting_reindeers > 0)
        {
            pthread_cond_wait(&reindeer_cond, &reindeer_mutex);
        }

        pthread_mutex_unlock(&reindeer_mutex);
    }

    return NULL;
}
