#include "common.h"

int server_queue_id;
int client_queue_id;
short int client_id;

key_t generate_key();
int send_init(key_t client_key);
Commands convert_command(char *command);

void handle_list();
void handle_to_one(int other_id, char *message);
void handle_to_all(char *message);
void handle_stop();

void handle_server_response();
void check_incoming_messages();

int main(int argc, char *argv[])
{
    key_t client_key = generate_key();

    server_queue_id = msgget(SERVER_KEY, DEFAULT_MSG_FLAG);
    client_queue_id = msgget(client_key, IPC_CREAT | 0666);

    client_id = send_init(client_key);

    if ((client_id) == -1)
    {
        printf("Max clients exceeded\n");
        return 0;
    }

    struct sigaction action;
    action.sa_handler = &handle_stop;
    sigaction(SIGINT, &action, NULL);

    char line_buffer[LINE_BUFFER_LENGTH];
    while (true)
    {
        check_incoming_messages();

        fgets(line_buffer, sizeof(line_buffer), stdin);

        if (strcmp(line_buffer, "\n") == 0)
        {
            continue;
        }

        char *command_str = strtok(line_buffer, DELIMITER);
        Commands command = convert_command(command_str);

        char message[MAX_MSG_SIZE] = "";
        char *tmp;

        switch (command)
        {
        case LIST:
            handle_list();

            break;
        case TO_ALL:
            while ((tmp = strtok(NULL, DELIMITER)) != NULL)
            {
                strcat(message, tmp);
                strcat(message, " ");
            }

            handle_to_all(message);

            continue;
        case TO_ONE:
            int other_id = atoi(strtok(NULL, DELIMITER));

            while ((tmp = strtok(NULL, DELIMITER)) != NULL)
            {
                strcat(message, tmp);
                strcat(message, " ");
            }
            handle_to_one(other_id, message);

            continue;
        case STOP:
            handle_stop();

            break;
        default:
            printf("Unknown command\n");

            continue;
        }
    }

    return 0;
}

key_t generate_key()
{
    srand(time(NULL));

    int key_seed = rand() % MAX_PROJ_ID + 1;

    return ftok(HOME_PATH, key_seed);
}

int send_init(key_t client_key)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->mtype = INIT;
    strcpy(msg_buffer->mtext, "INIT");
    msg_buffer->client_key = client_key;

    MsgBuffer *response_buffer = malloc(sizeof(MsgBuffer));

    msgsnd(server_queue_id, msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);
    msgrcv(client_queue_id, response_buffer, MSG_SIZE, 0, DEFAULT_MSG_FLAG);

    int client_id = response_buffer->client_id;

    free(msg_buffer);
    free(response_buffer);

    return client_id;
}

void handle_list()
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->mtype = LIST;
    strcpy(msg_buffer->mtext, "LIST");

    msgsnd(server_queue_id, msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);
    free(msg_buffer);

    handle_server_response();
}

void handle_to_one(int other_id, char *message)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->other_id = other_id;
    strcpy(msg_buffer->mtext, message);
    msg_buffer->mtype = TO_ONE;

    msgsnd(server_queue_id, msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);

    free(msg_buffer);
}

void handle_to_all(char *message)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    strcpy(msg_buffer->mtext, message);
    msg_buffer->mtype = TO_ALL;

    msgsnd(server_queue_id, msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);

    free(msg_buffer);
}

void handle_stop()
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->mtype = STOP;
    strcpy(msg_buffer->mtext, "STOP");
    msg_buffer->client_id = client_id;

    msgsnd(server_queue_id, msg_buffer, MSG_SIZE, DEFAULT_MSG_FLAG);
    msgctl(client_queue_id, IPC_RMID, NULL);

    free(msg_buffer);
    exit(0);
}

void handle_server_response()
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msgrcv(client_queue_id, msg_buffer, MSG_SIZE, RECEIVE_PRIORITY_FLAG, DEFAULT_MSG_FLAG);

    switch (msg_buffer->mtype)
    {
    case STOP:
        handle_stop();
        break;
    case LIST:
    case TO_ONE:
    case TO_ALL:
        printf("%s\n", msg_buffer->mtext);
        break;
    default:
        break;
    }

    free(msg_buffer);
}

void check_incoming_messages()
{
    struct msqid_ds *q_buffer = malloc(sizeof(struct msqid_ds));
    msgctl(client_queue_id, IPC_STAT, q_buffer);

    while (q_buffer->msg_qnum > 0)
    {
        handle_server_response();
        msgctl(client_queue_id, IPC_STAT, q_buffer);
    }

    free(q_buffer);
}

Commands convert_command(char *command)
{
    if (strcmp(command, "LIST") == 0)
    {
        return LIST;
    }
    if (strcmp(command, "2ALL") == 0)
    {
        return TO_ALL;
    }
    if (strcmp(command, "2ONE") == 0)
    {
        return TO_ONE;
    }
    if (strcmp(command, "STOP") == 0)
    {
        return STOP;
    }

    return INVALID;
}
