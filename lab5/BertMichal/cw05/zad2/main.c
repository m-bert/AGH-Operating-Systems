#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define BUFFER_SIZE 64

double f(double x)
{
    return 4 / (x * x + 1);
}

double rectangle_area(double rect_width, double midpoint)
{
    return rect_width * f(midpoint);
}

bool get_parameters(int argc, char *argv[], double *rect_width, int *processes_amount);
double calculate_area(double rect_width, int processes_amount);
double calculate_fragment(double rect_width, int processes_amount, int n);

double calculate_time(struct timespec start_time, struct timespec end_time);

int main(int argc, char *argv[])
{
    double rect_width;
    int processes_amount;

    if (!get_parameters(argc, argv, &rect_width, &processes_amount))
    {
        return -1;
    }

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    double result = calculate_area(rect_width, processes_amount);

    clock_gettime(CLOCK_REALTIME, &end_time);
    double time_elapsed = calculate_time(start_time, end_time);

    printf("==========================================\n");
    printf("Rectangle width:\t\t%.15lf\n", rect_width);
    printf("Number of processes:\t%d\n", processes_amount);
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
    double result = 0.0;

    int *output_pipes = calloc(processes_amount, sizeof(int));

    for (int i = 0; i < processes_amount; ++i)
    {
        int fd[2];
        pipe(fd);

        pid_t new_process_pid = fork();

        switch (new_process_pid)
        {
        case -1: // Fork failed
            break;
        case 0: // In child process
            close(fd[0]);

            double fragment_area = calculate_fragment(rect_width, processes_amount, i);

            char child_buffer[BUFFER_SIZE];
            sprintf(child_buffer, "%lf", fragment_area);
            write(fd[1], child_buffer, sizeof(child_buffer));

            close(fd[1]);
            exit(0);

        default: // In parent process
            close(fd[1]);
            output_pipes[i] = fd[0];

            break;
        }
    }

    while (wait(NULL) > 0)
    {
        // Waiting for children...
    }

    char buffer[BUFFER_SIZE];

    for (int i = 0; i < processes_amount; ++i)
    {
        read(output_pipes[i], buffer, BUFFER_SIZE);
        result += atof(buffer);
    }

    free(output_pipes);

    return result;
}

double calculate_fragment(double rect_width, int processes_amount, int n)
{
    const double fragment_width = 1.0 / (double)processes_amount;

    const double start = fragment_width * (double)n;
    const double end = fragment_width * (double)(n + 1);

    double midpoint = start + rect_width / 2.0;
    double result = 0.0;

    while (midpoint < end)
    {
        result += rectangle_area(rect_width, midpoint);

        midpoint += rect_width;
    }

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
