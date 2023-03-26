#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>

void set_signal();
void signal_handler(int sig);
void send_tasks(int tasks_no, char *tasks[]);

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Too few arguments\n");
		return -1;
	}

	set_signal();

	send_tasks(argc, argv);

	return 0;
}

void set_signal()
{
	struct sigaction action;

	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = &signal_handler;

	sigaction(SIGUSR1, &action, NULL);
}

void signal_handler(int sig)
{
	// Empty handler to ignore deafult behaviour upon receiving SIGUSR1
}

void send_tasks(int argc, char *argv[])
{
	const int catcher_pid = atoi(argv[1]);
	union sigval value;

	// Mark SIGUSR1 as signal to be waited for before sending next task
	sigset_t wait_for;
	int sig_no;
	sigaddset(&wait_for, SIGUSR1);

	for (int i = 2; i < argc; ++i)
	{
		int task = atoi(argv[i]);

		if (task < 1 || task > 5)
		{
			// Filter wrong tasks
			continue;
		}

		value.sival_int = task;
		sigqueue(catcher_pid, SIGUSR1, value);

		sigwait(&wait_for, &sig_no);
	}

	return;
}
