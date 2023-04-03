#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

bool get_parameters(int argc, char *argv[], double *rect_width, int *processes_amount);
double calculate_area(double rect_width, int processes_amount);

int main(int argc, char *argv[])
{

    double rect_width;
    int processes_amount;

    if (!get_parameters(argc, argv, &rect_width, &processes_amount))
    {
        return -1;
    }

    mkfifo("./tmp_pipe", 0666);

    double result = calculate_area(rect_width, processes_amount);
    printf("Area under the curve is %lf\n", result);

    unlink("./tmp_pipe");

    return 0;
}

bool get_parameters(int argc, char *argv[], double *rect_width, int *processes_amount)
{
    if (argc < 3)
    {
        printf("Too few arguments!\n");
        return false;
    }

    *processes_amount = atoi(argv[2]);

    if (*processes_amount <= 0)
    {
        printf("Number of processes has to be positive!\n");
        return false;
    }

    *rect_width = atof(argv[1]);

    if (*rect_width > (1.0 / *processes_amount))
    {
        printf("Rectangle width has to be <= (1.0 / processes_amount)\n");
        return false;
    }

    return true;
}

double calculate_area(double rect_width, int processes_amount)
{
    double result = 0.0;

    char arg1[32], arg2[32];

    sprintf(arg1, "%lf", rect_width);
    sprintf(arg2, "%d", processes_amount);

    for (int i = 0; i < processes_amount; ++i)
    {
        pid_t new_process_pid = fork();

        switch (new_process_pid)
        {
        case -1: // Fork failed
            break;
        case 0: // In child process
            char arg3[32];
            sprintf(arg3, "%d", i);

            char *av[] = {arg1,
                          arg2,
                          arg3, NULL};

            execv("./fragment_calculator", av);

            break;
        default: // In parent process
            break;
        }
    }

    int fifo = open("./tmp_pipe", O_RDONLY);

    char line_buffer[1024];

    int already_read = 0;

    while (already_read < processes_amount)
    {
        size_t read_amount = read(fifo, line_buffer, 1024);
        line_buffer[read_amount] = 0;

        char *buffer = strtok(line_buffer, "\n");

        while (buffer != NULL)
        {
            result += atof(buffer);
            ++already_read;

            buffer = strtok(NULL, "\n");
        }
    }

    close(fifo);

    return result;
}
