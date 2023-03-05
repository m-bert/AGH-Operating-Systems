#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib.h"

void perform_action(WordCounter *word_counter, char *command, char *arg1, char *arg2);

int main()
{
    WordCounter *word_counter = NULL;

    const char delimiter[] = " ";

    char line_buffer[1024];

    while (fgets(line_buffer, sizeof(line_buffer), stdin) != NULL)
    {
        line_buffer[strcspn(line_buffer, "\n")] = 0; // Get rid of \n

        char *command = strtok(line_buffer, delimiter);
        char *arg1 = strtok(NULL, delimiter);
        char *arg2 = strtok(NULL, delimiter);

        if (strcmp(command, "init") == 0)
        {
            word_counter = init_counter(atoi(arg1));
        }

        if (strcmp(command, "count") == 0)
        {
            perform_counting(word_counter, arg1);
        }

        if (strcmp(command, "show") == 0)
        {
            char *element = get_element_at(word_counter, atoi(arg1));

            if (element == NULL)
            {
                printf("Element not found\n");
                continue;
            }

            printf("%s\n", element);
        }

        if (strcmp(command, "delete") == 0 && strcmp(arg1, "index") == 0)
        {
            remove_element_at(word_counter, atoi(arg2));
        }

        if (strcmp(command, "destroy") == 0)
        {
            destroy(word_counter);
        }
    }

    return 0;
}
