#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <string.h>

double f(double x)
{
    return 4 / (x * x + 1);
}

bool get_parameters(int argc, char *argv[], double *rect_width, int *processes_amount);
double calculate_area(double rect_width, int processes_amount);
double calculate_fragment(double rect_width, int processes_amount, int n);
double rectangle_area(double rect_width, double midpoint);

int main(int argc, char *argv[])
{
    // w = 1/(nk)

    double rect_width;
    int processes_amount;

    if (!get_parameters(argc, argv, &rect_width, &processes_amount))
    {
        return -1;
    }

    double result = calculate_area(rect_width, processes_amount);

    printf("Area under the curve is %lf\n", result);

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

            char child_buffer[32];
            sprintf(child_buffer, "%lf", fragment_area);

            write(fd[1], child_buffer, sizeof(child_buffer));

            exit(0);

        default: // In parent process
            close(fd[1]);

            waitpid(new_process_pid, NULL, 0);

            char parent_buffer[32];

            read(fd[0], parent_buffer, sizeof(parent_buffer));

            result += atof(parent_buffer);

            break;
        }
    }

    return result;
}

double calculate_fragment(double rect_width, int processes_amount, int n)
{
    double fragment_width = 1.0 / (double)processes_amount;

    double start = fragment_width * (double)n;
    double end = fragment_width * (double)(n + 1);

    double midpoint = start + rect_width / 2.0;

    double result = 0.0;

    while (midpoint < end)
    {
        printf("Start = %lf, End = %lf, midpoint = %lf\n", start, end, midpoint);
        result += rectangle_area(rect_width, midpoint);

        midpoint += rect_width;
    }

    return result;
}

double rectangle_area(double rect_width, double midpoint)
{
    return rect_width * f(midpoint);
}
