#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

typedef enum VARIANT
{
	SIGINFO,
	RESETHAND,
	NODEFER,
	INVALID
} Variant;

Variant choose_variant(const char *variant);

void siginfo_handler(int sig, siginfo_t *info, void *ucontext);
void resethand_handler(int sig);
void nodefer_stop_handler(int sig);
void nodefer_int_handler(int sig);

void siginfo_test();
void resethand_test();
void nodefer_test();

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
		printf("Valid arguments are: siginfo, resethand, nodefer\n");
		return -1;
	}

	switch (variant)
	{
	case SIGINFO:
		siginfo_test();
		break;
	case RESETHAND:
		resethand_test();
		break;
	case NODEFER:
		nodefer_test();
		break;
	default:
		break;
	}

	return 0;
}

Variant choose_variant(const char *variant)
{
	if (strcmp(variant, "siginfo") == 0)
	{
		return SIGINFO;
	}

	if (strcmp(variant, "resethand") == 0)
	{
		return RESETHAND;
	}

	if (strcmp(variant, "nodefer") == 0)
	{
		return NODEFER;
	}

	return INVALID;
}

void siginfo_test()
{
	struct sigaction action;
	struct sigaction old_action;

	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = &siginfo_handler;

	sigaction(SIGTSTP, &action, &old_action);

	while (1)
	{
		sleep(1);
	}
}

void siginfo_handler(int sig, siginfo_t *info, void *ucontext)
{
	printf("\n==============================\n");
	printf("Signal No: %d\n", info->si_signo);
	printf("PID: %d\n\n", info->si_pid);

	printf("User: %d\n", info->si_uid);
	printf("User time: %ld\n", info->si_utime);
	printf("Signal code: %d\n", info->si_code);
	printf("==============================\n");
}

void resethand_test()
{
	struct sigaction action;
	struct sigaction old_action;

	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESETHAND;
	action.sa_handler = &resethand_handler;

	sigaction(SIGINT, &action, &old_action);

	while (1)
	{
		sleep(1);
	}
}

void resethand_handler(int sig)
{
	printf("Testing SA_RESETHAND flag\n");
}

void nodefer_test()
{
	// Set action for SIGTSTP
	struct sigaction stop_action;
	struct sigaction old_stop_action;

	sigemptyset(&stop_action.sa_mask);
	stop_action.sa_flags = SA_NODEFER;
	stop_action.sa_handler = &nodefer_stop_handler;

	sigaction(SIGTSTP, &stop_action, &old_stop_action);

	// Set action for SIGINT
	struct sigaction int_action;
	struct sigaction old_int_action;

	sigemptyset(&int_action.sa_mask);
	int_action.sa_flags = 0;
	int_action.sa_handler = &nodefer_int_handler;

	sigaction(SIGINT, &int_action, &old_int_action);

	// Wait for SIGTSTP signal
	sleep(10);
}

void nodefer_stop_handler(int sig)
{
	printf("\nReceived SIGTSTP\n");

	while (1)
	{
		sleep(1);
	}
}
void nodefer_int_handler(int sig)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGTSTP);
	sigprocmask(SIG_SETMASK, &mask, NULL);
}
