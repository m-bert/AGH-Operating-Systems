#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

typedef enum Variant
{
    READ,
    WRITE,
    INVALID
} Variant;

Variant choose_variant(const char *variant);
int read_variant();
int write_variant();

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        return -1;
    }

    Variant variant = choose_variant(argv[1]);

    switch (variant)
    {
    case READ:
        return read_variant();
    case WRITE:
        return write_variant();
    default:
        printf("Invalid variant\n");
        return -1;
    }
}

Variant choose_variant(const char *variant)
{
    if (strcmp(variant, "read") == 0)
    {
        return READ;
    }

    if (strcmp(variant, "write") == 0)
    {
        return WRITE;
    }

    return INVALID;
}

int read_variant()
{
    char buffer[256];

    FILE *read_pipe = popen("fortune", "r");

    if (!read_pipe)
    {
        printf("Couldn't open pipe\n");
        return -1;
    }

    while (fgets(buffer, sizeof(char) * 256, read_pipe) != NULL)
    {
        printf("%s\n", buffer);
    }

    pclose(read_pipe);

    return 0;
}

int write_variant()
{
    FILE *write_pipe = popen("cowsay", "w");

    if (!write_pipe)
    {
        printf("Couldn't open pipe\n");
        return -1;
    }

    fputs("Moo", write_pipe);

    pclose(write_pipe);

    return 0;
}
