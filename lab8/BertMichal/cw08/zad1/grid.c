#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

char *create_grid()
{
    return malloc(sizeof(char) * GRID_WIDTH * GRID_HEIGHT);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < GRID_HEIGHT; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < GRID_WIDTH; ++j)
        {
            if (grid[i * GRID_WIDTH + j])
            {
                mvprintw(i, j * 2, "■");
                // mvprintw(i, j * 2, "🤣");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
        grid[i] = rand() % 2 == 0;
}

void *is_alive(void *arg)
{
    struct sigaction action;
    action.sa_handler = empty_handler;
    sigaction(SIGUSR1, &action, NULL);

    f_args *args = (f_args *)arg;

    char *background = *args->background;
    char *foreground = *args->foreground;

    while (true)
    {
        pause();

        int count = 0;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                if (i == 0 && j == 0)
                {
                    continue;
                }
                int r = args->row + i;
                int c = args->col + j;
                if (r < 0 || r >= GRID_HEIGHT || c < 0 || c >= GRID_WIDTH)
                {
                    continue;
                }
                if (args->foreground[GRID_WIDTH * r + c])
                {
                    count++;
                }
            }
        }

        if (args->foreground[args->row * GRID_WIDTH + args->col])
        {
            args->background[args->row * GRID_WIDTH + args->col] = (count == 2 || count == 3);
        }
        else
        {
            args->background[args->row * GRID_WIDTH + args->col] = count == 3;
        }
    }

    return NULL;
}

void empty_handler(int signum) {}