#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#define HOME_PATH getenv("HOME")
#define SERVER_KEY_SEED 0
#define SERVER_KEY ftok(HOME_PATH, SERVER_KEY_SEED)

#define MAX_PROJ_ID 255

#define CLIENT_ID_OFFSET 10000
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 1024
#define RECEIVE_PRIORITY_FLAG -10
#define DEFAULT_MSG_FLAG 0

#define LINE_BUFFER_LENGTH 256
#define DELIMITER " \n"

typedef struct MsgBuffer
{
    long mtype;
    char mtext[MAX_MSG_SIZE];

    key_t client_key;
    int client_id;
    int other_id;
} MsgBuffer;

#define MSG_SIZE sizeof(MsgBuffer) - sizeof(long)

typedef enum Commands
{
    INIT = 1,
    STOP,
    LIST,
    TO_ALL,
    TO_ONE,
    INVALID
} Commands;

#endif