#include <stdio.h>
#include <string.h>
#include "../zad1/lib.h"

void perform_action(WordCounter *word_counter, char *command, char *arg1, char *arg2);

int main()
{
    WordCounter *word_counter;

    const char delimiter[] = " ";

    char line_buffer[1024];

    while (fgets(line_buffer, sizeof(line_buffer), stdin) != NULL)
    {
        line_buffer[strcspn(line_buffer, "\n")] = 0; // Get rid of \n

        char *command = strtok(line_buffer, delimiter);
        char *arg1 = strtok(NULL, delimiter);
        char *arg2 = strtok(NULL, delimiter);

        perform_action(word_counter, command, arg1, arg2);
    }

    return 0;
}

void perform_action(WordCounter *word_counter, char *command, char *arg1, char *arg2)
{
    if (strcmp(command, "init") == 0)
    {
        printf("Initializing...\n");
        return;
    }

    if (strcmp(command, "count") == 0)
    {
        printf("Counting words...\n");
        return;
    }

    if (strcmp(command, "show") == 0)
    {
        printf("Show...\n");
        return;
    }

    if (strcmp(command, "delete") == 0)
    {
        printf("Deleting...\n");
        return;
    }

    if (strcmp(command, "destroy") == 0)
    {
        printf("Destroying...\n");
        return;
    }
}