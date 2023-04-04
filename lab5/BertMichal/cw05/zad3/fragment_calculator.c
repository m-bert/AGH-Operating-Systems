#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PIPE_PATH "./tmp_pipe"
#define BUFFER_SIZE 64

double f(double x)
{
    return 4 / (x * x + 1);
}

double rectangle_area(double rect_width, double midpoint)
{
    return rect_width * f(midpoint);
}

int main(int argc, char *argv[])
{
    // Get parameters
    const double rect_width = atof(argv[0]);
    const int rectangles_amount = atoi(argv[1]);
    const int n = atoi(argv[2]);

    //
    const double fragment_width = 1.0 / (double)rectangles_amount;
    const double start = fragment_width * (double)n;
    const double end = fragment_width * (double)(n + 1);

    //
    double midpoint = start + rect_width / 2.0;
    double result = 0.0;

    while (midpoint < end)
    {
        result += rectangle_area(rect_width, midpoint);

        midpoint += rect_width;
    }

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    sprintf(buffer, "%lf\n", result);

    int fifo = open(PIPE_PATH, O_WRONLY);
    write(fifo, buffer, strlen(buffer));
    close(fifo);

    free(buffer);

    return 0;
}
