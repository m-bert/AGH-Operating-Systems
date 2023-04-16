#include "common.h"

mqd_t server_queue_descriptor;
mqd_t client_queues[MAX_CLIENTS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

void exit_handler();
int find_next_id();

void add_client(char *client_queue_name);
void remove_client(int client_id);
void send_clients_list(int client_id);
void send_to_one(int client_id, char *message);
void send_to_all(int sender_id, char *message);

int main(int argc, char *argv[])
{
    struct mq_attr mq_attributes;
    mq_attributes.mq_msgsize = MAX_MSG_SIZE;
    mq_attributes.mq_maxmsg = MAX_CLIENTS;
    server_queue_descriptor = mq_open(SERVER_QUEUE_PATH, O_RDWR | O_CREAT, 0666, &mq_attributes);

    struct sigaction action;
    action.sa_handler = &exit_handler;
    sigaction(SIGINT, &action, NULL);

    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));

    while (true)
    {
        mq_receive(server_queue_descriptor, (char *)msg_buffer, MSG_SIZE, NULL);

        switch (msg_buffer->mtype)
        {
        case INIT:
            add_client(msg_buffer->mtext);
            break;
        case LIST:
            printf("SWITCH%d\n", msg_buffer->client_id);
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

        mq_send(client_queues[i], (char *)msg_buffer, MAX_MSG_SIZE, PRIORITY_HIGHEST);
        mq_receive(server_queue_descriptor, (char *)response_buffer, MSG_SIZE, NULL);
    }

    free(msg_buffer);
    free(response_buffer);

    mq_close(server_queue_descriptor);
    mq_unlink(SERVER_QUEUE_PATH);

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

void add_client(char *client_queue_name)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));

    int new_client_id = find_next_id();
    mqd_t client_queue_descriptor = mq_open(client_queue_name, O_RDWR);

    if (new_client_id != -1)
    {
        client_queues[new_client_id] = client_queue_descriptor;
    }

    msg_buffer->client_id = new_client_id;
    msg_buffer->mtype = INIT;

    mq_send(client_queue_descriptor, (char *)msg_buffer, MAX_MSG_SIZE, INIT);

    free(msg_buffer);

    return;
}

void remove_client(int client_id)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->mtype = STOP;
    msg_buffer->client_id = client_id;

    mq_send(client_queues[client_id], (char *)msg_buffer, MAX_MSG_SIZE, STOP);
    mq_close(client_queues[client_id]);

    client_queues[client_id] = -1;

    free(msg_buffer);
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

    MsgBuffer *msg_buffer = calloc(1, sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->mtype = LIST;
    strcpy(msg_buffer->mtext, active_clients_ids);
    printf("%d \n", client_id);

    mq_send(client_queues[client_id], (char *)msg_buffer, MAX_MSG_SIZE, LIST);
    perror("SEND");

    free(msg_buffer);
}

void send_to_one(int client_id, char *message)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->mtype = TO_ONE;
    strcpy(msg_buffer->mtext, message);

    mq_send(client_queues[client_id], (char *)msg_buffer, MAX_MSG_SIZE, TO_ONE);

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
        mq_send(client_queues[i], (char *)msg_buffer, MAX_MSG_SIZE, TO_ALL);
    }

    free(msg_buffer);
}
