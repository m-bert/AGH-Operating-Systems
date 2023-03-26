#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <time.h>

typedef enum State
{
	START,
	NUMBERS,
	TIME,
	COUNTER,
	LOOP_TIME,
	FINISHED
} State;

State state;
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

	state = START;
	tasks_counter = 0;

	set_signal();

	while (state != FINISHED)
	{
		if (state == LOOP_TIME)
		{
			print_time();
			sleep(1);
		}
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
	State new_state = (State)info->si_value.sival_int;
	++tasks_counter;

	int sender_pid = info->si_pid;

	switch (new_state)
	{
	case NUMBERS:
		print_numbers();
		break;
	case TIME:
		print_time();
		break;
	case COUNTER:
		print_tasks_counter();
		break;
	case LOOP_TIME:
	case FINISHED:
		break;
	default:
		break;
	}

	state = new_state;

	kill(sender_pid, SIGUSR1);
}

void print_numbers()
{
	printf("\n==================================\n");
	for (int i = 1; i <= 100; ++i)
	{
		printf("%d ", i);
	}
	printf("\n==================================\n");

	return;
}

void print_time()
{
	time_t rawtime;
	struct tm *timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	printf("\n==================================\n");
	printf("Current local time and date: %s", asctime(timeinfo));
	printf("==================================\n");

	return;
}

void print_tasks_counter()
{
	printf("\n==================================\n");
	printf("Total number of tasks: %d\n", tasks_counter);
	printf("==================================\n");

	return;
}
