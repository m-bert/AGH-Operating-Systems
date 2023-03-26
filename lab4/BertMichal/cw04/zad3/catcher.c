#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>

bool running;

void set_signal();
void signal_handler(int sig, siginfo_t *info, void *ucontext);

int main(int argc, char *argv[])
{
	printf("\n==================================\n");
	printf("Starting catcher with PID:%d\n", getpid());
	printf("==================================\n");

	running = true;

	set_signal();

	while (running)
	{
		sleep(1);
	}

	printf("\n==================================\n");
	printf("Ending catcher\n");
	printf("==================================\n");

	return 0;
}

void set_signal()
{
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = &signal_handler;
	sigaction(SIGUSR1, &action, NULL);
}

void signal_handler(int sig, siginfo_t *info, void *ucontext)
{
	int task = info->si_value.sival_int;
	int sender_pid = info->si_pid;
	printf("Signal received: %d with value = %d\n", sig, task);

	switch (task)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		running = false;
		break;
	}

	kill(sender_pid, SIGUSR1);
}