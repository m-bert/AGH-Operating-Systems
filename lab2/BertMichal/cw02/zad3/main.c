#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    DIR *dir = opendir("./");

    if (!dir)
    {
        printf("Couldn't open current directory\n");
        return -1;
    }

    long long total_size = 0;

    struct stat st;
    struct dirent *dir_entry;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        stat(dir_entry->d_name, &st);

        if (S_ISDIR(st.st_mode))
        {
            continue;
        }

        printf("{Filename: %s, Size: %ld}\n", dir_entry->d_name, st.st_size);

        total_size += st.st_size;
    }

    printf("Total size: %lld\n", total_size);

    closedir(dir);

    return 0;
}