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

int main()
{
	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	struct sigaction action;
	action.sa_handler = empty_handler;
	action.sa_flags = 0;
	sigaction(SIGUSR1, &action, NULL);

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

	pthread_t *threads = calloc(MAX_CELLS, sizeof(pthread_t));
	f_args **args = malloc(MAX_CELLS * sizeof(f_args *));

	for (int i = 0; i < MAX_CELLS; ++i)
	{
		args[i] = malloc(sizeof(f_args));
		args[i]->row = i / GRID_WIDTH;
		args[i]->col = i % GRID_WIDTH;
		args[i]->foreground = &foreground;
		args[i]->background = &background;

		pthread_create(&threads[i], NULL, is_alive, args[i]);
	}

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		for (int i = 0; i < MAX_CELLS; ++i)
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

	for (int i = 0; i < MAX_CELLS; ++i)
	{
		free(args[i]);
	}

	free(args);
	free(threads);
	free(tmp);

	return 0;
}
