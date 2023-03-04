#include <stdio.h>
#include <string.h>

int main()
{
    const char delimiter[] = " ";

    char line_buffer[1024];

    while (fgets(line_buffer, sizeof(line_buffer), stdin) != NULL)
    {
        char *word = strtok(line_buffer, delimiter);
        printf("%s", word);

        word = strtok(NULL, delimiter);
        printf("%s", word);
    }

    return 0;
}