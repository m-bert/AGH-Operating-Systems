#include "common.h"

mqd_t server_queue_descriptor;
mqd_t client_queues[MAX_CLIENTS] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
FILE *log_file;

void exit_handler();
int find_next_id();
void perform_log(int cmd, int client_id, char *message);

void add_client(char *client_queue_name);
void remove_client(int client_id);
void send_clients_list(int client_id);
void send_to_one(int client_id, char *message);
void send_to_all(int sender_id, char *message);

int main(int argc, char *argv[])
{
    log_file = fopen("log.txt", "w");

    struct mq_attr mq_attributes;
    mq_attributes.mq_msgsize = MAX_MSG_SIZE;
    mq_attributes.mq_maxmsg = MAX_CLIENTS;
    server_queue_descriptor = mq_open(SERVER_QUEUE_PATH, O_RDWR | O_CREAT, 0666, &mq_attributes);

    struct sigaction action;
    action.sa_handler = &exit_handler;
    sigaction(SIGINT, &action, NULL);

    while (true)
    {
        char msg[MAX_MSG_SIZE] = "";

        mq_receive(server_queue_descriptor, msg, MAX_MSG_SIZE, NULL);

        int cmd = atoi(strtok(msg, DELIMITER));
        int client_id = atoi(strtok(NULL, DELIMITER));
        int other_id = atoi(strtok(NULL, DELIMITER));

        char *tmp;
        char message[MAX_MSG_SIZE] = "";

        while ((tmp = strtok(NULL, DELIMITER)) != NULL)
        {
            strcat(message, tmp);
            strcat(message, " ");
        }

        perform_log(cmd, client_id, message);

        switch (cmd)
        {
        case INIT:
            message[strlen(message) - 1] = '\0'; // Trim \n
            add_client(message);
            break;
        case LIST:
            send_clients_list(client_id);
            break;
        case TO_ONE:
            send_to_one(other_id, message);
            break;
        case TO_ALL:
            send_to_all(client_id, message);
            break;
        case STOP:
            remove_client(client_id);
        default:
            break;
        }
    }

    return 0;
}

void exit_handler()
{
    char msg[MAX_MSG_SIZE] = "";
    sprintf(msg, "%d %s", STOP, "STOP");

    char *response = calloc(MAX_MSG_SIZE, sizeof(char));

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_queues[i] == -1)
        {
            continue;
        }

        mq_send(client_queues[i], msg, MAX_MSG_SIZE, PRIORITY_HIGHEST);
        mq_receive(server_queue_descriptor, response, MAX_MSG_SIZE, NULL);
        remove_client(i);
    }

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
    char msg[MAX_MSG_SIZE] = "";
    int new_client_id = find_next_id();
    mqd_t client_queue_descriptor = mq_open(client_queue_name, O_RDWR);

    if (new_client_id != -1)
    {
        client_queues[new_client_id] = client_queue_descriptor;
    }

    sprintf(msg, "%d", new_client_id);

    mq_send(client_queue_descriptor, msg, MAX_MSG_SIZE, INIT);

    return;
}

void remove_client(int client_id)
{
    char msg[MAX_MSG_SIZE] = "";
    sprintf(msg, "%d %s", Q_CLOSED, "Q_CLOSED");

    mq_send(client_queues[client_id], msg, MAX_MSG_SIZE, Q_CLOSED);
    mq_close(client_queues[client_id]);

    client_queues[client_id] = -1;
}

void send_clients_list(int client_id)
{
    char active_clients_ids[128] = "";

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

    char msg[MAX_MSG_SIZE] = "";
    sprintf(msg, "%d %s", LIST, active_clients_ids);

    mq_send(client_queues[client_id], msg, MAX_MSG_SIZE, LIST);
}

void send_to_one(int client_id, char *message)
{
    char msg[MAX_MSG_SIZE] = "";
    sprintf(msg, "%d %s", TO_ONE, message);

    mq_send(client_queues[client_id], msg, MAX_MSG_SIZE, TO_ONE);
}

void send_to_all(int sender_id, char *message)
{
    char msg[MAX_MSG_SIZE] = "";
    sprintf(msg, "%d %s", TO_ALL, message);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (client_queues[i] == -1 || i == sender_id)
        {
            continue;
        }
        mq_send(client_queues[i], msg, MAX_MSG_SIZE, TO_ALL);
    }
}

void perform_log(int cmd, int client_id, char *message)
{
    char log[2 * MAX_MSG_SIZE];
    char date[64];

    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo = localtime(&rawtime);

    sprintf(date, "%s", asctime(timeinfo));
    date[strlen(date) - 1] = '\0'; // Remove \n

    sprintf(log, "[%s][ID: %d][%d] %s\n", date, client_id, cmd, message);
    printf("%s\n", log);
    fwrite(log, sizeof(char), strlen(log), log_file);
}