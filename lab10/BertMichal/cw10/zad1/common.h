#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include <sys/un.h>
#include <sys/epoll.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_MSG 512
#define MAX_NICK 32
#define MAX_PATH 128
#define LINE_BUFFER_LENGTH 256
#define DELIMITER " \n"

const char *FULL = "FULL";
const char *CONNECTED = "CONNECTED";

const char *INIT = "INIT";
const char *TO_ALL = "2ALL";
const char *TO_ONE = "2ONE";
const char *LIST = "LIST";
const char *STOP = "STOP";
const char *PING = "PING";

typedef enum Event_Type
{
    SOCKET = 1,
    CLIENT
} Event_Type;

typedef struct Event_Data
{
    Event_Type event_type;
    int socket_fd;
} Event_Data;
