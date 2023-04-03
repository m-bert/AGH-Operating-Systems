#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

#define PIPE_PATH "./tmp_pipe"
#define CALC_PATH "./fragment_calculator"
#define BUFFER_SIZE 1024
#define ARG_SIZE 32
#define DELIMITER "\n"

bool get_parameters(int argc, char *argv[], double *rect_width, int *processes_amount);
double calculate_area(double rect_width, int processes_amount);
double calculate_time(struct timespec start_time, struct timespec end_time);

int main(int argc, char *argv[])
{

    double rect_width;
    int processes_amount;

    if (!get_parameters(argc, argv, &rect_width, &processes_amount))
    {
        return -1;
    }

    mkfifo(PIPE_PATH, 0666);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    double result = calculate_area(rect_width, processes_amount);

    clock_gettime(CLOCK_REALTIME, &end_time);
    double time_elapsed = calculate_time(start_time, end_time);

    remove(PIPE_PATH);

    printf("==========================================\n");
    printf("Rectangle width:\t\t%.15lf\n", rect_width);
    printf("Number of programms:\t%d\n", processes_amount);
    printf("Area:\t\t\t\t\t%.15lf\n", result);
    printf("Time elapsed:\t\t\t%.5lfs\n", time_elapsed);
    printf("==========================================\n");

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
    char arg1[ARG_SIZE], arg2[ARG_SIZE];

    sprintf(arg1, "%.20lf", rect_width);
    sprintf(arg2, "%d", processes_amount);

    for (int i = 0; i < processes_amount; ++i)
    {
        pid_t new_process_pid = fork();

        switch (new_process_pid)
        {
        case -1: // Fork failed
            break;
        case 0: // In child process
            char arg3[ARG_SIZE];
            sprintf(arg3, "%d", i);

            char *av[] = {arg1, arg2, arg3, NULL};

            execv(CALC_PATH, av);

            break;
        default: // In parent process
            break;
        }
    }

    double result = 0.0;
    int received_results = 0;

    int fifo = open(PIPE_PATH, O_RDONLY);
    char line_buffer[BUFFER_SIZE];

    while (received_results < processes_amount)
    {
        size_t read_amount = read(fifo, line_buffer, BUFFER_SIZE);
        // Terminate string earlier so that it doesn't contain any garbage that would unwillingly increment received_results
        line_buffer[read_amount] = '\0';

        char *result_buffer = strtok(line_buffer, DELIMITER);

        while (result_buffer != NULL)
        {
            result += atof(result_buffer);
            ++received_results;

            result_buffer = strtok(NULL, DELIMITER);
        }
    }

    close(fifo);

    return result;
}

double calculate_time(struct timespec start_time, struct timespec end_time)
{

    double s_delta = (double)end_time.tv_sec - (double)start_time.tv_sec;
    double ns_delta = (double)end_time.tv_nsec - (double)start_time.tv_nsec;

    double seconds = s_delta;
    double nanoseconds = ns_delta;

    if (ns_delta < 0)
    {
        seconds -= 1.0;
        nanoseconds = pow(10, 9) + ns_delta;
    }

    nanoseconds /= pow(10, 9);

    return seconds + nanoseconds;
}
