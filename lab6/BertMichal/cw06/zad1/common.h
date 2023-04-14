#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define HOME_PATH getenv("HOME")
#define SERVER_KEY_SEED 0
#define MAX_PROJ_ID 255

#define CLIENT_ID_OFFSET 10000

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 1024

typedef struct MsgBuffer
{
    long mtype;
    char mtext[MAX_MSG_SIZE];

    key_t queue_key;
    int client_id;
    int other_id;
} MsgBuffer;

typedef enum Commands
{
    INIT,
    STOP,
    LIST,
    TO_ALL,
    TO_ONE
} Commands;

#endif