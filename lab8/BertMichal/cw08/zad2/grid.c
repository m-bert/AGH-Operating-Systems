#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

char *create_grid()
{
    return malloc(sizeof(char) * MAX_CELLS);
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
    for (int i = 0; i < MAX_CELLS; ++i)
        grid[i] = rand() % 2 == 0;
}

void *is_alive(void *arg)
{
    f_args *args = (f_args *)arg;
    char *foreground = *args->foreground;
    char *background = *args->background;

    while (true)
    {
        foreground = *args->foreground;
        background = *args->background;

        for (int thread_cell_index = 0; thread_cell_index < args->max_cells_no; ++thread_cell_index)
        {
            int count = 0;

            int row = args->rows[thread_cell_index];
            int col = args->cols[thread_cell_index];

            if (row == -1 || col == -1)
            {
                break;
            }

            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (i == 0 && j == 0)
                    {
                        continue;
                    }

                    int current_row = row + i;
                    int current_col = col + j;

                    if (current_row < 0 || current_row >= GRID_HEIGHT || current_col < 0 || current_col >= GRID_WIDTH)
                    {
                        continue;
                    }

                    if (foreground[GRID_WIDTH * current_row + current_col])
                    {
                        ++count;
                    }
                }
            }

            int current_index = row * GRID_WIDTH + col;

            if (foreground[current_index])
            {
                background[current_index] = count == 2 || count == 3;
            }
            else
            {
                background[current_index] = count == 3;
            }
        }

        pause();
    }

    return NULL;
}

void empty_handler(int signum) {}