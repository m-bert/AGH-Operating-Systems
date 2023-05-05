#pragma once
#include <stdbool.h>

typedef struct f_args
{
    int row;
    int col;
    char **foreground;
    char **background;
} f_args;

#define GRID_WIDTH 30
#define GRID_HEIGHT 30

#define MAX_CELLS (GRID_WIDTH * GRID_HEIGHT)

char *create_grid();
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
void *is_alive(void *arg);

void empty_handler(int signum);