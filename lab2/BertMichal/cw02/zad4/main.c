#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ftw.h>

int traverse_functions(const char *path, const struct stat *st, int flag);

long long total_size = 0;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        printf("Usage: ./main <dir_name>\n");

        return -1;
    }

    const char *dir_name = argv[1];

    if (ftw(dir_name, traverse_functions, 10) != 0)
    {
        printf("Traversing failed\n");
        return -1;
    }

    printf("Total size: %lld\n", total_size);

    return 0;
}

int traverse_functions(const char *path, const struct stat *st, int flag)
{
    if (S_ISDIR(st->st_mode))
    {
        return 0;
    }

    printf("{Filename: %s, Size: %ld}\n", path, st->st_size);
    total_size += st->st_size;

    return 0;
}