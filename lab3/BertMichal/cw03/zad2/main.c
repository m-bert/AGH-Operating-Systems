#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        return -1;
    }

    const char *path = argv[1];

    printf("%s ", argv[0]);

    fflush(stdout);

    execl("/bin/ls", "ls", path, NULL);

    return 0;
}