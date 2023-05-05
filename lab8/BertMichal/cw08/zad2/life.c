#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

int *create_empty_array(int size);
void init_cells(int *rows, int *cols, int threads_amount, int starting_index);

int main(int argc, char *argv[])
{
	// Data validation
	if (argc < 2)
	{
		printf("Too few arguments!\n");
		exit(-1);
	}

	int threads_amount = atoi(argv[1]);

	if (threads_amount <= 0)
	{
		printf("Number of threads must be positive!\n");
		exit(-1);
	}
	else if (threads_amount > MAX_CELLS)
	{
		threads_amount = MAX_CELLS;
	}

	// Init
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	struct sigaction action;
	action.sa_handler = empty_handler;
	sigaction(SIGUSR1, &action, NULL);

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

	int max_cells_per_thread = MAX_CELLS / threads_amount + 1;

	pthread_t *threads = calloc(threads_amount, sizeof(pthread_t));
	f_args **args = malloc(threads_amount * sizeof(f_args *));

	for (int i = 0; i < threads_amount; ++i)
	{
		args[i] = malloc(sizeof(f_args));
		args[i]->rows = create_empty_array(max_cells_per_thread);
		args[i]->cols = create_empty_array(max_cells_per_thread);
		args[i]->max_cells_no = max_cells_per_thread;

		init_cells(args[i]->rows, args[i]->cols, threads_amount, i);

		args[i]->foreground = &foreground;
		args[i]->background = &background;

		pthread_create(&threads[i], NULL, is_alive, args[i]);
	}

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		for (int i = 0; i < threads_amount; ++i)
		{
			pthread_kill(threads[i], SIGUSR1);
		}

		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	free(tmp);

	for (int i = 0; i < MAX_CELLS; ++i)
	{
		free(args[i]->rows);
		free(args[i]->cols);
		free(args[i]);
	}

	free(args);
	free(threads);

	return 0;
}

int *create_empty_array(int size)
{
	int *array = malloc(size * sizeof(int));

	for (int i = 0; i < size; ++i)
	{
		array[i] = -1;
	}

	return array;
}

void init_cells(int *rows, int *cols, int threads_amount, int starting_index)
{
	int current_cell = starting_index;
	int index = 0;

	while (current_cell < MAX_CELLS)
	{
		rows[index] = current_cell / GRID_WIDTH;
		cols[index] = current_cell % GRID_WIDTH;

		current_cell += threads_amount;
		++index;
	}
}