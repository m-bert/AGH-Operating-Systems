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

bool read_dir(const char *path, const char *pattern, const int pattern_length);

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

    if (!read_dir(starting_dir, pattern, pattern_length))
    {
        return -1;
    }

    return 0;
}

bool read_dir(const char *path, const char *pattern, const int pattern_length)
{

    DIR *dir = opendir(path);

    if (!dir)
    {
        return false;
    }

    struct stat st;
    struct dirent *dir_entry;
    char *buffer = malloc(sizeof(char) * (pattern_length + 1));

    if (!buffer)
    {
        perror("Failed to allocate memory\n");

        closedir(dir);
        return false;
    }

    while ((dir_entry = readdir(dir)) != NULL)
    {
        // Avoid traversing . and ..
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
        {
            continue;
        }

        const unsigned int current_path_length = strlen(dir_entry->d_name) + strlen(path) + 1;
        char *current_path = malloc(sizeof(char) * (current_path_length + 1));

        if (!current_path)
        {
            perror("Failed to allocate memory\n");

            free(buffer);
            return false;
        }

        sprintf(current_path, "%s/%s", path, dir_entry->d_name);

        if (stat(current_path, &st) == -1)
        {
            return false;
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
                return false;

            case 0: // In child process
                bool return_value = read_dir(current_path, pattern, pattern_length);

                free(buffer);
                free(current_path);

                closedir(dir);
                return return_value;

            default: // In parent process
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
            return false;
        }

        fread(buffer, sizeof(char), pattern_length, f);

        fclose(f);

        if (strcmp(buffer, pattern) == 0)
        {
            printf("%s\n", current_path);
        }

        free(current_path);
    }

    free(buffer);

    closedir(dir);

    return true;
}