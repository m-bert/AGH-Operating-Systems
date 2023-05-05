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

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

	pthread_t *threads = calloc(GRID_WIDTH * GRID_HEIGHT, sizeof(pthread_t));
	for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
	{
		struct f_args *args = malloc(sizeof(args));
		args->row = i / GRID_WIDTH;
		args->col = i % GRID_WIDTH;
		args->foreground = &foreground;
		args->background = &background;

		pthread_create(&threads[i], NULL, is_alive, args);
	}

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		// Step simulation
		// update_grid(foreground, background);
		for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
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

	return 0;
}
