#include "common.h"

#define MAX_CLIENTS 10

typedef struct Client
{
    char *nick;
    int socket_id;
    Event_Data *event_data;
} Client;

char *UNIX_PATH;
int PORT, WEB_SOCKET_FD, UNIX_SOCKET_FD, EPOLL_FD;
Client *clients[MAX_CLIENTS];

int find_client_index();

void *connections_handler();
void *ping();

void init_web_socket();
void init_unix_socket();

int add_client(char *nick, int client_fd);
void remove_client(int client_fd);
void list_users(int client_fd);
void send_to_one(char *nick, char *message);
void send_to_all(char *message);

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Too few arguments!\n");
        return -1;
    }

    PORT = atoi(argv[1]);
    UNIX_PATH = argv[2];

    EPOLL_FD = epoll_create1(0);

    init_web_socket();
    init_unix_socket();

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        clients[i] = calloc(1, sizeof(Client));
        clients[i]->nick = "";
        clients[i]->socket_id = -1;
    }

    pthread_t connection_thread, ping_thread;
    pthread_create(&connection_thread, NULL, connections_handler, NULL);
    pthread_create(&ping_thread, NULL, ping, NULL);

    pthread_join(connection_thread, NULL);
    pthread_join(ping_thread, NULL);

    return 0;
}

int find_client_index()
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i]->socket_id == -1)
        {
            return i;
        }
    }

    return -1;
}

int add_client(char *nick, int client_fd)
{
    int new_client_id;
    if ((new_client_id = find_client_index()) < 0)
    {
        return -1;
    }

    clients[new_client_id]->nick = nick;
    clients[new_client_id]->socket_id = client_fd;

    return new_client_id;
}

void remove_client(int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i]->socket_id == client_fd)
        {
            clients[i]->nick = "";
            clients[i]->socket_id = -1;

            close(client_fd);

            return;
        }
    }

    return;
}

void list_users(int client_fd)
{
    char active_users[512] = "";

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i]->socket_id != -1)
        {
            strcat(active_users, clients[i]->nick);
            strcat(active_users, " ");
        }
    }

    send(client_fd, active_users, strlen(active_users), 0);
}

void send_to_one(char *nick, char *message)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (strcmp(nick, clients[i]->nick) == 0)
        {
            send(clients[i]->socket_id, message, strlen(message), 0);
        }
    }
}

void send_to_all(char *message)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        send(clients[i]->socket_id, message, strlen(message), 0);
    }
}

void init_web_socket()
{
    WEB_SOCKET_FD = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in web_address;
    web_address.sin_family = AF_INET;
    web_address.sin_port = htons(PORT);
    web_address.sin_addr.s_addr = INADDR_ANY;

    bind(WEB_SOCKET_FD, (struct sockaddr *)&web_address, sizeof(web_address));

    listen(WEB_SOCKET_FD, MAX_CLIENTS);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = WEB_SOCKET_FD;

    Event_Data *event_data = calloc(1, sizeof(Event_Data));
    event_data->event_type = SOCKET;
    event_data->socket_fd = WEB_SOCKET_FD;
    event.data.ptr = event_data;

    epoll_ctl(EPOLL_FD, EPOLL_CTL_ADD, WEB_SOCKET_FD, &event);
}

void init_unix_socket()
{
    UNIX_SOCKET_FD = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un unix_address;
    unix_address.sun_family = AF_UNIX;
    strcpy(unix_address.sun_path, UNIX_PATH);

    bind(UNIX_SOCKET_FD, (struct sockaddr *)&unix_address, sizeof(unix_address));

    listen(UNIX_SOCKET_FD, MAX_CLIENTS);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = UNIX_SOCKET_FD;

    Event_Data *event_data = calloc(1, sizeof(Event_Data));
    event_data->event_type = SOCKET;
    event_data->socket_fd = UNIX_SOCKET_FD;
    event.data.ptr = event_data;

    epoll_ctl(EPOLL_FD, EPOLL_CTL_ADD, UNIX_SOCKET_FD, &event);
}

void *connections_handler()
{
    struct epoll_event events[MAX_CLIENTS];

    while (true)
    {
        int read_amount = epoll_wait(EPOLL_FD, events, MAX_CLIENTS, -1);

        for (int i = 0; i < read_amount; ++i)
        {
            Event_Data *event_data = (Event_Data *)events[i].data.ptr;

            if (event_data->event_type == SOCKET)
            {
                int client_fd;

                if ((client_fd = accept(event_data->socket_fd, NULL, NULL)) < 0)
                {
                    continue;
                }

                char *nick = calloc(MAX_NICK, sizeof(char));
                recv(client_fd, (char *)nick, sizeof(nick), 0);

                int new_client_id;

                if ((new_client_id = add_client(nick, client_fd)) < 0)
                {
                    send(client_fd, FULL, strlen(FULL), 0);
                    close(client_fd);

                    continue;
                }

                send(client_fd, CONNECTED, strlen(CONNECTED), 0);

                struct epoll_event event;
                event.events = EPOLLIN | EPOLLPRI;
                event.data.fd = client_fd;

                clients[new_client_id]->event_data = calloc(1, sizeof(Event_Data));
                clients[new_client_id]->event_data->event_type = CLIENT;
                clients[new_client_id]->event_data->socket_fd = client_fd;
                event.data.ptr = clients[new_client_id]->event_data;

                epoll_ctl(EPOLL_FD, EPOLL_CTL_ADD, client_fd, &event);
            }
            else if (event_data->event_type == CLIENT)
            {
                char *msg = calloc(MAX_MSG, sizeof(char));
                recv(event_data->socket_fd, (char *)msg, MAX_MSG, 0);

                if (strlen(msg) == 0)
                {
                    continue;
                }

                char *command = strtok(msg, DELIMITER);

                if (strcmp(command, LIST) == 0)
                {
                    list_users(event_data->socket_fd);
                }

                if (strcmp(command, STOP) == 0)
                {
                    remove_client(event_data->socket_fd);
                }

                if (strcmp(command, TO_ALL) == 0)
                {
                    char message[MAX_MSG] = "";
                    char *tmp;

                    while ((tmp = strtok(NULL, DELIMITER)) != NULL)
                    {
                        strcat(message, tmp);
                        strcat(message, " ");
                    }

                    send_to_all(message);
                }

                if (strcmp(command, TO_ONE) == 0)
                {
                    char *other_nick = strtok(NULL, DELIMITER);

                    char message[MAX_MSG] = "";
                    char *tmp;

                    while ((tmp = strtok(NULL, DELIMITER)) != NULL)
                    {
                        strcat(message, tmp);
                        strcat(message, " ");
                    }

                    send_to_one(other_nick, message);
                }
            }
        }
    }

    return NULL;
}

void *ping()
{
    return NULL;
}