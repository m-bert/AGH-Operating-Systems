#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <time.h>

bool running;
int tasks_counter;

void set_signal();
void signal_handler(int sig, siginfo_t *info, void *ucontext);

void print_numbers();
void print_time();
void print_tasks_counter();
void loop_time();

int main(int argc, char *argv[])
{
	printf("\n==================================\n");
	printf("Starting catcher with PID:%d\n", getpid());
	printf("==================================\n");

	running = true;
	tasks_counter = 0;

	set_signal();

	while (running)
	{
		// sleep(1);
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
		print_numbers();
		break;
	case 2:
		print_time();
		break;
	case 3:
		print_tasks_counter();
		break;
	case 4:
		break;
	case 5:
		running = false;
		break;
	}

	kill(sender_pid, SIGUSR1);
}

void print_numbers()
{
	for (int i = 1; i <= 100; ++i)
	{
		printf("%d ", i);
	}
	printf("\n");

	return;
}

void print_time()
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("Current local time and date: %s", asctime(timeinfo));

	return;
}

void print_tasks_counter()
{
	printf("Total number of tasks: %d", ++tasks_counter);

	return;
}

void loop_time()
{
	// while ()
	// {
	// }
}
