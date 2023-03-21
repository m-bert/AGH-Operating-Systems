#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        return -1;
    }

    printf("Initial PID: %d\n", getpid());
    printf("=========================================\n");

    const int processes_amount = atoi(argv[1]);
    int statloc;

    for (int i = 0; i < processes_amount; ++i)
    {
        pid_t new_process_pid = fork();

        if (new_process_pid == 0)
        {
            printf("Child process: | PID: %d | PPID: %d\n", (int)getpid(), (int)getppid());
            return 0;
        }

        wait(&statloc);
    }

    printf("=========================================\n");
    printf("Number of processes: %d [Current PID: %d]\n", processes_amount, getpid());

    return 0;
}