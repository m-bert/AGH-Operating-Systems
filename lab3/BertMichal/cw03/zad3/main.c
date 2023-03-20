#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>

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

    if (!dir)
    {
        perror("Failed to open directory\n");
        return;
    }

    struct stat st;
    struct dirent *dir_entry;

    while ((dir_entry = readdir(dir)) != NULL)
    {
        char *buffer = calloc((pattern_length + 1), sizeof(char));

        if (!buffer)
        {
            perror("Failed to allocate memory\n");

            closedir(dir);
            return;
        }

        // Avoid traversing . and ..
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
        {
            free(buffer);
            continue;
        }

        const unsigned int current_path_length = strlen(dir_entry->d_name) + strlen(path) + 1;
        char *current_path = malloc(sizeof(char) * (current_path_length + 1));

        if (!current_path)
        {
            perror("Failed to allocate memory\n");

            free(buffer);
            closedir(dir);
            return;
        }

        sprintf(current_path, "%s/%s", path, dir_entry->d_name);

        if (stat(current_path, &st) == -1)
        {
            free(buffer);
            free(current_path);
            closedir(dir);
            return;
        }

        if (S_ISDIR(st.st_mode))
        {
            pid_t new_process_pid = fork();

            switch (new_process_pid)
            {
            case -1: // Fork failed
                perror("Fork failed\n");

                free(buffer);
                free(current_path);
                closedir(dir);
                return;

            case 0: // In child process
                read_dir(current_path, pattern, pattern_length);

                free(buffer);
                free(current_path);
                closedir(dir);
                return;

            default: // In parent process
                free(buffer);
                free(current_path);
                continue;
            }
        }

        FILE *f = fopen(current_path, "r");

        if (!f)
        {
            free(buffer);
            free(current_path);
            closedir(dir);
            return;
        }

        fread(buffer, sizeof(char), pattern_length, f);

        fclose(f);

        if (strcmp(buffer, pattern) == 0)
        {
            printf("%s\n", current_path);
        }

        free(buffer);
        free(current_path);
    }

    closedir(dir);

    while (wait(NULL) > 0)
    {
        // This loop is only to wait for the child processes, so that terminal will show its default prompt after
        //  all processes have finished
    }

    return;
}