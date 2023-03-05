#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <inttypes.h>
#include <dlfcn.h>

#include "lib.h"

void print_time(struct tms time_start, struct tms time_end, clock_t real_start, clock_t real_end);

int main(int argc, char **argv)
{
#ifdef DLL
    void *handle = dlopen("./liblib.so", RTLD_LAZY);

    WordCounter *(*init_counter)(int) = dlsym(handle, "init_counter");
    void (*perform_counting)(WordCounter *, char *) = dlsym(handle, "perform_counting");
    char *(*get_element_at)(WordCounter *, int) = dlsym(handle, "get_element_at");
    void (*remove_element_at)(WordCounter *, int) = dlsym(handle, "remove_element_at");
    void (*destroy)(WordCounter *) = dlsym(handle, "destroy");

    if (handle == NULL)
    {
        printf("Failed to load liblib.so\n");
        return -1;
    }
#endif

    WordCounter *word_counter = NULL;

    const char uninitialized_error_message[] = "Error: word_counter not initialized!\n";

    const char delimiter[] = " ";
    char line_buffer[1024];
    char *command, *arg1, *arg2;

    struct tms start_time, end_time;
    clock_t real_start, real_end;

    while (fgets(line_buffer, sizeof(line_buffer), stdin) != NULL)
    {
        // Get rid of \n
        line_buffer[strcspn(line_buffer, "\n")] = 0;

        // Get args from line
        command = strtok(line_buffer, delimiter);
        arg1 = strtok(NULL, delimiter);
        arg2 = strtok(NULL, delimiter);

        // Perform actions
        if (strcmp(command, "init") == 0)
        {
            if (word_counter != NULL)
            {
                printf("Error: word_counter is already initialized!\n");
                continue;
            }

            if (arg1 == NULL)
            {
                printf("You have to provide size for the word counter!\n");
                continue;
            }

            int size = atoi(arg1);

            if (size <= 0)
            {
                printf("You have to provide a positive size!\n");
                continue;
            }

            real_start = times(&start_time);
            word_counter = init_counter(size);
            real_end = times(&end_time);
        }

        else if (strcmp(command, "count") == 0)
        {
            if (word_counter == NULL)
            {
                printf(uninitialized_error_message);
                continue;
            }

            real_start = times(&start_time);
            perform_counting(word_counter, arg1);
            real_end = times(&end_time);
        }

        else if (strcmp(command, "show") == 0)
        {
            if (word_counter == NULL)
            {
                printf(uninitialized_error_message);
                continue;
            }

            real_start = times(&start_time);
            char *element = get_element_at(word_counter, atoi(arg1));

            if (element == NULL)
            {
                printf("Element not found\n");
                continue;
            }

            printf("%s\n", element);
            real_end = times(&end_time);
        }

        else if (strcmp(command, "delete") == 0 && strcmp(arg1, "index") == 0)
        {
            if (word_counter == NULL)
            {
                printf(uninitialized_error_message);
                continue;
            }

            real_start = times(&start_time);
            remove_element_at(word_counter, atoi(arg2));
            real_end = times(&end_time);
        }

        else if (strcmp(command, "destroy") == 0)
        {
            if (word_counter == NULL)
            {
                printf(uninitialized_error_message);
                continue;
            }

            real_start = times(&start_time);
            destroy(word_counter);
            word_counter = NULL;
            real_end = times(&end_time);
        }

        else
        {
            printf("Unknown command\n");
            continue;
        }

        print_time(start_time, end_time, real_start, real_end);
    }

#ifdef DLL
    dlclose(handle);
#endif

    return 0;
}

void print_time(struct tms start_time, struct tms end_time, clock_t real_start, clock_t real_end)
{
    clock_t real_time = real_end - real_start;
    clock_t user_time = end_time.tms_utime - start_time.tms_utime;
    clock_t system_time = end_time.tms_stime - start_time.tms_stime;

    printf("real_time: %.15lf\n", (double)(real_time * 1000 / sysconf(_SC_CLK_TCK)));
    printf("user_time: %.15lf\n", (double)(user_time * 1000 / sysconf(_SC_CLK_TCK)));
    printf("system_time: %.15lf\n", (double)(system_time * 1000 / sysconf(_SC_CLK_TCK)));
}