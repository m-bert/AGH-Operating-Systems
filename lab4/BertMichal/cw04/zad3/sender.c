#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>

void set_signal();
void signal_handler(int sig);

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Too few arguments\n");
		return -1;
	}

	set_signal();

	sigset_t wait_for;
	int sig_no;
	sigaddset(&wait_for, SIGUSR1);

	const int catcher_pid = atoi(argv[1]);
	union sigval value;

	for (int i = 2; i < argc; ++i)
	{
		int task = atoi(argv[i]);
		printf("SENDING: %d\n", task);

		if (task < 1 || task > 5)
		{
			continue;
		}

		value.sival_int = task;
		sigqueue(catcher_pid, SIGUSR1, value);

		sigwait(&wait_for, &sig_no);
	}

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
}
