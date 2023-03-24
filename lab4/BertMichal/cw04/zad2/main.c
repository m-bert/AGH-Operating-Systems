#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void siginfo_handler(int sig, siginfo_t *info, void *ucontext);
void tmp_handler(int sig);

void siginfo_test();

int main(int argc, char *argv[])
{
	siginfo_test();

	while (1)
	{
		sleep(10);
	}

	return 0;
}

void siginfo_test()
{
	struct sigaction action;
	struct sigaction old_action;

	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;
	action.sa_handler = &siginfo_handler;

	sigaction(SIGTSTP, &action, &old_action);
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
void tmp_handler(int sig)
{
	printf("SIGINFO\n");
}