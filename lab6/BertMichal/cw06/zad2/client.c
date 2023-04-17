
#include "common.h"

mqd_t server_queue_descriptor;
mqd_t client_queue_descriptor;
short int client_id;
char client_queue_name[MAX_QUEUE_NAME];

void generate_queue_name();
int send_init();
void close_queues();
void format_message(char *message, int cmd, int client_id, int other_id, char *content);
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

    char line_buffer[LINE_BUFFER_LENGTH] = "";
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

            break;

        case STOP:
            handle_stop();
            break;
        default:
            printf("Unknown command\n");
            break;
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
    char msg[MAX_MSG_SIZE] = "";
    format_message(msg, INIT, -1, -1, client_queue_name);
    mq_send(server_queue_descriptor, msg, MAX_MSG_SIZE, INIT);

    char response[MAX_MSG_SIZE] = "";
    mq_receive(client_queue_descriptor, response, MAX_MSG_SIZE, NULL);

    int client_id = atoi(response);

    return client_id;
}

void handle_list()
{
    char msg[MAX_MSG_SIZE] = "";
    format_message(msg, LIST, client_id, -1, "LIST");

    mq_send(server_queue_descriptor, msg, MAX_MSG_SIZE, LIST);

    handle_server_response();
}

void handle_to_one(int other_id, char *message)
{
    char msg[MAX_MSG_SIZE] = "";
    format_message(msg, TO_ONE, client_id, other_id, message);

    mq_send(server_queue_descriptor, msg, MAX_MSG_SIZE, TO_ONE);
}

void handle_to_all(char *message)
{
    char msg[MAX_MSG_SIZE] = "";
    format_message(msg, TO_ALL, client_id, -1, message);

    mq_send(server_queue_descriptor, msg, MAX_MSG_SIZE, TO_ALL);
}

void handle_stop()
{
    char msg[MAX_MSG_SIZE] = "";
    format_message(msg, STOP, client_id, -1, "STOP");

    mq_send(server_queue_descriptor, msg, MAX_MSG_SIZE, STOP);

    handle_server_response();
}

void handle_server_response()
{
    char msg[MAX_MSG_SIZE] = "";
    mq_receive(client_queue_descriptor, msg, MAX_MSG_SIZE, NULL);

    char *tmp;
    int cmd = atoi(strtok(msg, DELIMITER));

    char msg_content[MAX_MSG_SIZE] = "";
    while ((tmp = strtok(NULL, DELIMITER)) != NULL)
    {
        strcat(msg_content, tmp);
        strcat(msg_content, " ");
    }

    switch (cmd)
    {
    case STOP:

        handle_stop();
        break;
    case Q_CLOSED:
        close_queues();
        exit(0);
        break;
    case LIST:
    case TO_ONE:
    case TO_ALL:
        printf("%s\n", msg_content);
        break;
    default:
        break;
    }
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

void close_queues()
{
    mq_close(server_queue_descriptor);
    mq_close(client_queue_descriptor);
    mq_unlink(client_queue_name);
}

void format_message(char *message, int cmd, int client_id, int other_id, char *content)
{
    sprintf(message, "%d %d %d %s", cmd, client_id, other_id, content);
}