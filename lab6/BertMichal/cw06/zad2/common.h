#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define SERVER_QUEUE_PATH "/serverqueue"

#define MAX_QUEUE_NAME 128

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 1024

#define LINE_BUFFER_LENGTH 256
#define DELIMITER " \n"

#define PRIORITY_HIGHEST 10

typedef enum Commands
{
    INVALID = 1,
    TO_ONE,
    TO_ALL,
    LIST,
    Q_CLOSED,
    STOP,
    INIT,
} Commands;
typedef struct MsgBuffer
{
    long mtype;
    char mtext[MAX_MSG_SIZE];

    mqd_t client_queue_name;
    int client_id;
    int other_id;
} MsgBuffer;

#endif