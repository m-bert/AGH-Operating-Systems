#include "common.h"

typedef enum Connection_Type
{
    WEB,
    LOCAL
} Connection_Type;

char nick[MAX_NICK];
Connection_Type connection_type;
char *server_ip;
char *UNIX_PATH;

Event_Data *event_data;
pthread_t read_thread;

int PORT,
    SOCKET_FD, EPOLL_FD;

bool running = true;

void init_socket();

void handle_stop();
void *read_messages();

int main(int argc, char *argv[])
{
    UNIX_PATH = calloc(MAX_PATH, sizeof(char));

    if (argc < 4)
    {
        printf("Too few arguments!\n");
        return -1;
    }

    if (strlen(argv[1]) >= MAX_NICK)
    {
        printf("Too long nickname!\n");
        return -1;
    }

    strcpy(nick, argv[1]);

    if (strcmp(argv[2], "WEB") == 0)
    {
        if (argc < 5)
        {
            printf("Provide port!\n");
            return -1;
        }

        connection_type = WEB;
        server_ip = argv[3];
        PORT = atoi(argv[4]);
    }
    else if (strcmp(argv[2], "LOCAL") == 0)
    {
        connection_type = LOCAL;
        strcpy(UNIX_PATH, argv[3]);
    }
    else
    {
        printf("Invalid connection type!\n");
        return -1;
    }

    init_socket();

    char *response = calloc(MAX_MSG, sizeof(char));
    send(SOCKET_FD, nick, strlen(nick), 0);

    recv(SOCKET_FD, (char *)response, MAX_MSG, 0);

    if (strcmp(response, FULL) == 0)
    {
        free(response);
        printf("Server full!\n");
        return 0;
    }

    free(response);

    pthread_create(&read_thread, NULL, read_messages, NULL);

    struct sigaction action;
    action.sa_handler = &handle_stop;
    sigaction(SIGINT, &action, NULL);

    char line_buffer[LINE_BUFFER_LENGTH] = "";
    while (true)
    {
        fgets(line_buffer, sizeof(line_buffer), stdin);

        if (strcmp(line_buffer, "\n") == 0)
        {
            continue;
        }

        send(SOCKET_FD, (char *)line_buffer, strlen(line_buffer), 0);

        char *tmp = strtok(line_buffer, DELIMITER);

        if (strcmp(tmp, STOP) == 0)
        {
            running = false;
            break;
        }
    }

    pthread_join(read_thread, NULL);
    return 0;
}

void init_socket()
{
    if (connection_type == WEB)
    {
        SOCKET_FD = socket(AF_INET, SOCK_STREAM, 0);

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        inet_pton(AF_INET, server_ip, &addr.sin_addr);

        connect(SOCKET_FD, (struct sockaddr *)&addr, sizeof(addr));
    }
    else
    {
        SOCKET_FD = socket(AF_UNIX, SOCK_STREAM, 0);

        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, UNIX_PATH);

        connect(SOCKET_FD, (struct sockaddr *)&addr, sizeof(addr));
    }

    EPOLL_FD = epoll_create1(0);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = SOCKET_FD;

    event_data = calloc(1, sizeof(Event_Data));
    event_data->event_type = SOCKET;
    event_data->socket_fd = SOCKET_FD;
    event.data.ptr = event_data;

    epoll_ctl(EPOLL_FD, EPOLL_CTL_ADD, SOCKET_FD, &event);
}

void *read_messages()
{
    struct epoll_event events[10];

    while (running)
    {
        int read_amount = epoll_wait(EPOLL_FD, events, 10, -1);

        for (int i = 0; i < read_amount; ++i)
        {
            Event_Data *event_data = (Event_Data *)events[i].data.ptr;

            if (event_data->event_type == SOCKET)
            {
                char msg[MAX_MSG] = "";
                recv(event_data->socket_fd, (char *)msg, MAX_MSG, 0);

                if (strcmp(msg, SERVER_STOPPED))
                {
                    handle_stop();
                }

                if (strlen(msg) > 0)
                {
                    printf("%s\n", msg);
                }
            }
        }
    }

    send(SOCKET_FD, STOP, strlen(STOP), 0);

    free(event_data);
    close(SOCKET_FD);

    return NULL;
}

void handle_stop()
{
    running = false;
    send(SOCKET_FD, STOP, strlen(STOP), 0);
    pthread_join(read_thread, NULL);

    exit(0);
}
