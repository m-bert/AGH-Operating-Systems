#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

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

int main(int argc, char *argv[])
{
    printf("=======================\n");
    printf("EXEC TEST\n");

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
