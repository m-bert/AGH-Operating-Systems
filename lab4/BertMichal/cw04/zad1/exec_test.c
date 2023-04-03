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

void check_pending_signals();

int main(int argc, char *argv[])
{
    printf("=======================\n");
    printf("EXEC TEST\n");

    Variant variant = choose_variant(argv[1]);

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
