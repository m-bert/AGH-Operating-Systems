#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>

typedef enum VARIANT
{
	IGNORE,
	HANDLER,
	MASK,
	PENDING,
	INVALID
} Variant;

Variant choose_variant(const char *variant);

void set_signal_action(const Variant variant);
void signal_handler(int sig_no);
void check_pending_signals();

bool test_inheritance(Variant variant, char *argv[]);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Too few arguments\n");
		return -1;
	}

	Variant variant = choose_variant(argv[1]);

	if (variant == INVALID)
	{
		printf("Invalid argument\n");
		return -1;
	}

	set_signal_action(variant);

	raise(SIGUSR1);

	if (variant == PENDING)
	{
		check_pending_signals();
	}

	if (!test_inheritance(variant, argv))
	{
		return -1;
	}

	return 0;
}

Variant choose_variant(const char *variant)
{
	if (strcmp(variant, "ignore") == 0)
	{
		return IGNORE;
	}

	if (strcmp(variant, "handler") == 0)
	{
		return HANDLER;
	}

	if (strcmp(variant, "mask") == 0)
	{
		return MASK;
	}

	if (strcmp(variant, "pending") == 0)
	{
		return PENDING;
	}

	return INVALID;
}

void set_signal_action(const Variant variant)
{
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	sigset_t mask;
	sigemptyset(&mask);

	switch (variant)
	{
	case IGNORE:
		action.sa_handler = SIG_IGN;
		sigaction(SIGUSR1, &action, NULL);

		break;

	case HANDLER:
		action.sa_handler = &signal_handler;
		sigaction(SIGUSR1, &action, NULL);

		break;

	case MASK:
	case PENDING:
		sigaddset(&mask, SIGUSR1);
		sigprocmask(SIG_SETMASK, &mask, NULL);

		break;

	default:
		break;
	}
}

void signal_handler(int sig_no)
{
	printf("Received signal %d\n", sig_no);
	return;
}

void check_pending_signals()
{
	sigset_t pending_signals;

	sigpending(&pending_signals);

	if (sigismember(&pending_signals, SIGUSR1))
	{
		printf("SIGUSR1 is pending\n");
	}
	else
	{
		printf("SIGUSR1 is not pending\n");
	}

	return;
}

bool test_inheritance(Variant variant, char *argv[])
{
	pid_t new_process_pid = fork();

	switch (new_process_pid)
	{
	case -1: // Fork failed
		printf("Fork failed\n");
		return false;

	case 0: // In child process
		if (variant == PENDING)
		{
			check_pending_signals();
			return true;
		}

		raise(SIGUSR1);

		return true;

	default: // In parent process
		wait(NULL);

		if (execv("./exec_test", argv) == -1)
		{
			printf("Failed to process exec\n");
			return false;
		}

		return true;
	}
}
