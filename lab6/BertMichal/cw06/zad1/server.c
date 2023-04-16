#include "common.h"

int server_queue_id;
int client_queues[MAX_CLIENTS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
FILE *log_file;

void exit_handler();
int find_next_id();
void perform_log(MsgBuffer *msg_buffer);

void add_client(int client_key);
void remove_client(int client_id);
void send_clients_list(int client_id);
void send_to_one(int client_id, char *message);
void send_to_all(int sender_id, char *message);

int main(int argc, char *argv[])
{
    log_file = fopen("log.txt", "w");

    server_queue_id = msgget(SERVER_KEY, IPC_CREAT | 0666);

    struct sigaction action;
    action.sa_handler = &exit_handler;
    sigaction(SIGINT, &action, NULL);

    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));

    while (true)
    {
        msgrcv(server_queue_id, msg_buffer, MSG_SIZE, RECEIVE_PRIORITY_FLAG, DEFAULT_MSG_FLAG);
        perform_log(msg_buffer);

        switch (msg_buffer->mtype)
        {
        case INIT:
            add_client(msg_buffer->client_key);
            break;
        case LIST:
            send_clients_list(msg_buffer->client_id);
            break;
        case TO_ONE:
            send_to_one(msg_buffer->other_id, msg_buffer->mtext);
            break;
        case TO_ALL:
            send_to_all(msg_buffer->client_id, msg_buffer->mtext);
            break;
        case STOP:
            remove_client(msg_buffer->client_id);
        default:
            break;
        }
    }

    return 0;
}

void exit_handler()
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    MsgBuffer *response_buffer = malloc(sizeof(MsgBuffer));

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_queues[i] == -1)
        {
            continue;
        }

        msg_buffer->client_id = i;
        msg_buffer->mtype = STOP;

        msgsnd(client_queues[i], msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);
        msgrcv(server_queue_id, response_buffer, MSG_SIZE, RECEIVE_PRIORITY_FLAG, DEFAULT_MSG_FLAG);
    }

    free(msg_buffer);
    free(response_buffer);

    msgctl(server_queue_id, IPC_RMID, NULL);

    fclose(log_file);
    exit(0);
}

int find_next_id()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_queues[i] == -1)
        {
            return i;
        }
    }

    return -1;
}

void add_client(int client_key)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));

    int new_client_id = find_next_id();
    int client_queue_id = msgget(client_key, DEFAULT_MSG_FLAG);

    if (new_client_id != -1)
    {
        client_queues[new_client_id] = client_queue_id;
    }

    msg_buffer->client_id = new_client_id;
    msg_buffer->mtype = INIT;

    msgsnd(client_queue_id, msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);

    free(msg_buffer);

    return;
}

void remove_client(int client_id)
{
    client_queues[client_id] = -1;
}

void send_clients_list(int client_id)
{
    char active_clients_ids[MAX_MSG_SIZE] = "";

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_queues[i] == -1)
        {
            continue;
        }

        char tmp[3] = "";
        sprintf(tmp, "%d ", i);

        strcat(active_clients_ids, tmp);
    }

    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->mtype = LIST;
    strcpy(msg_buffer->mtext, active_clients_ids);

    msgsnd(client_queues[client_id], msg_buffer, MAX_MSG_SIZE, DEFAULT_MSG_FLAG);

    free(msg_buffer);
}

void send_to_one(int client_id, char *message)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->mtype = TO_ONE;
    strcpy(msg_buffer->mtext, message);

    msgsnd(client_queues[client_id], msg_buffer, MAX_MSG_SIZE, DEFAULT_MSG_FLAG);

    free(msg_buffer);
}

void send_to_all(int sender_id, char *message)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->mtype = TO_ALL;
    strcpy(msg_buffer->mtext, message);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_queues[i] == -1 || i == sender_id)
        {
            continue;
        }

        msg_buffer->client_id = i;
        msgsnd(client_queues[i], msg_buffer, MAX_MSG_SIZE, DEFAULT_MSG_FLAG);
    }

    free(msg_buffer);
}

void perform_log(MsgBuffer *msg_buffer)
{
    char log[2 * MAX_MSG_SIZE];
    char date[64];

    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo = localtime(&rawtime);

    sprintf(date, "%s", asctime(timeinfo));
    date[strlen(date) - 1] = '\0'; // Remove \n

    sprintf(log, "[%s][ID: %d][%ld] %s\n", date, msg_buffer->client_id, msg_buffer->mtype, msg_buffer->mtext);
    printf("%s\n", log);
    fwrite(log, sizeof(char), strlen(log), log_file);
}
