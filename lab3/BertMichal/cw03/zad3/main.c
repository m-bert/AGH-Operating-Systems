#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#define MAX_PATTERN_LENGTH 255

void read_dir(const char *path, const char *pattern, const int pattern_length);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <dir> <pattern>\n", argv[0]);
        return -1;
    }

    const char *starting_dir = argv[1];
    const char *pattern = argv[2];
    const int pattern_length = strlen(pattern);

    if (pattern_length > MAX_PATTERN_LENGTH)
    {
        printf("Too long pattern\n");
        return -1;
    }

    read_dir(starting_dir, pattern, pattern_length);

    return 0;
}

void read_dir(const char *path, const char *pattern, const int pattern_length)
{

    DIR *dir = opendir(path);

    struct stat st;
    struct dirent *dir_entry;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        // Avoid traversing . and ..
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
        {
            continue;
        }

        const unsigned int current_path_length = strlen(dir_entry->d_name) + strlen(path) + 1;
        char current_path[current_path_length];
        sprintf(current_path, "%s/%s", path, dir_entry->d_name);

        stat(current_path, &st);

        if (S_ISDIR(st.st_mode))
        {
            read_dir(current_path, pattern, pattern_length);
            continue;
        }

        FILE *f = fopen(current_path, "r");
        char *buffer = malloc(sizeof(char) * pattern_length);
        fread(buffer, sizeof(char), pattern_length, f);
        fclose(f);

        printf("BUFFER %s\n", buffer);

        if (strcmp(buffer, pattern) == 0)
        {
            printf("HAS: %s\n", current_path);
        }

        free(buffer);
    }

    closedir(dir);
    return;
}