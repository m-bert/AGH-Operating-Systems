
#include "common.h"

mqd_t server_queue_descriptor;
mqd_t client_queue_descriptor;
short int client_id;
char client_queue_name[MAX_QUEUE_NAME];

void generate_queue_name();
int send_init();
Commands convert_command(char *command);

void handle_list();
void handle_to_one(int other_id, char *message);
void handle_to_all(char *message);
void handle_stop();

void handle_server_response();
void check_incoming_messages();

int main(int argc, char *argv[])
{
    generate_queue_name();

    struct mq_attr mq_attributes;
    mq_attributes.mq_msgsize = MAX_MSG_SIZE;
    mq_attributes.mq_maxmsg = MAX_CLIENTS;
    client_queue_descriptor = mq_open(client_queue_name, O_RDWR | O_CREAT, 0666, &mq_attributes);

    server_queue_descriptor = mq_open(SERVER_QUEUE_PATH, O_RDWR);
    client_id = send_init();

    if ((client_id) < 0)
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

void generate_queue_name()
{
    sprintf(client_queue_name, "/%d_mqd", getpid());
}

int send_init()
{
    MsgBuffer *msg_buffer = calloc(1, sizeof(MsgBuffer));
    msg_buffer->mtype = INIT;
    strcpy(msg_buffer->mtext, client_queue_name);

    MsgBuffer *response_buffer = calloc(1, sizeof(MsgBuffer));

    mq_send(server_queue_descriptor, (char *)msg_buffer, MAX_MSG_SIZE, INIT);
    mq_receive(client_queue_descriptor, (char *)response_buffer, MSG_SIZE, NULL);

    int client_id = response_buffer->client_id;

    free(msg_buffer);
    free(response_buffer);

    return client_id;
}

void handle_list()
{
    MsgBuffer *msg_buffer = calloc(1, sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    msg_buffer->mtype = LIST;

    printf("ID: %d\n", client_id);

    mq_send(server_queue_descriptor, (char *)msg_buffer, MAX_MSG_SIZE, LIST);

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

    mq_send(server_queue_descriptor, (char *)msg_buffer, MAX_MSG_SIZE, TO_ONE);

    free(msg_buffer);
}

void handle_to_all(char *message)
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->client_id = client_id;
    strcpy(msg_buffer->mtext, message);
    msg_buffer->mtype = TO_ALL;

    mq_send(server_queue_descriptor, (char *)msg_buffer, MAX_MSG_SIZE, TO_ALL);

    free(msg_buffer);
}

void handle_stop()
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    msg_buffer->mtype = STOP;
    msg_buffer->client_id = client_id;

    MsgBuffer *response_buffer = malloc(sizeof(MsgBuffer));

    mq_send(server_queue_descriptor, (char *)msg_buffer, MAX_MSG_SIZE, STOP);
    mq_receive(client_queue_descriptor, (char *)response_buffer, MSG_SIZE, NULL);

    mq_close(server_queue_descriptor);

    mq_close(client_queue_descriptor);
    mq_unlink(client_queue_name);

    free(msg_buffer);
    free(response_buffer);
    exit(0);
}

void handle_server_response()
{
    MsgBuffer *msg_buffer = malloc(sizeof(MsgBuffer));
    mq_receive(client_queue_descriptor, (char *)msg_buffer, MSG_SIZE, NULL);

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
    struct mq_attr *mq_attributes = malloc(sizeof(struct mq_attr));
    mq_getattr(client_queue_descriptor, mq_attributes);

    if (mq_attributes->mq_curmsgs > 0)
    {
        handle_server_response();
    }

    free(mq_attributes);
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
