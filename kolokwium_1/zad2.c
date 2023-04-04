
#include "zad2.h"

void mask()
{
    /*  Zamaskuj sygnał SIGUSR2, tak aby nie docierał on do procesu */

    sigset_t mask;
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    check_mask();
}

void process()
{
    /*  Stworz nowy process potomny i uruchom w nim program ./check_fork
        W procesie macierzystym:
            1. poczekaj 1 sekundę
            2. wyślij SIGUSR1 do procesu potomnego
            3. poczekaj na zakończenie procesu potomnego */

    pid_t new_pid = fork();

    switch (new_pid)
    {
    case -1: // Fail
        break;
    case 0: // Child
        execl("./check_fork", "check_fork", NULL);
        break;
    default: // Parent
        sleep(1);
        kill(new_pid, SIGUSR1);
        waitpid(new_pid, NULL, 0);
        break;
    }
}

int main()
{
    mask();
    process();

    return 0;
}