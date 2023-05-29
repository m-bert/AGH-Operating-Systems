#include "common.h"

#define MAX_CLIENTS 10

typedef struct Client
{
    char *nick;
    int socket_id;
    Event_Data *event_data;
    bool active;
} Client;

char *UNIX_PATH;
int PORT, WEB_SOCKET_FD, UNIX_SOCKET_FD, EPOLL_FD;
Client *clients[MAX_CLIENTS];
Event_Data *web_event_data, *unix_event_data;
bool running = true;

pthread_t connection_thread, ping_thread;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *log_file;

int find_client_index();

void *connections_handler();
void *ping();

void refresh_user(int client_fd);

void handle_stop();

void init_web_socket();
void init_unix_socket();

bool add_client(char *nick, int client_fd);
void remove_client(int client_fd);
void list_users(int client_fd);
void send_to_one(char *nick, char *message);
void send_to_all(char *message);

void perform_log(char *message);

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
        // clients[i]->nick = calloc(MAX_NICK, sizeof(char));
        clients[i]->socket_id = -1;
        clients[i]->active = false;
        clients[i]->event_data = NULL;
    }

    pthread_create(&connection_thread, NULL, connections_handler, NULL);
    pthread_create(&ping_thread, NULL, ping, NULL);

    struct sigaction action;
    action.sa_handler = &handle_stop;
    sigaction(SIGINT, &action, NULL);

    log_file = fopen("log.txt", "w");

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

bool add_client(char *nick, int client_fd)
{
    int new_client_id;
    if ((new_client_id = find_client_index()) < 0)
    {
        return false;
    }

    // clients[new_client_id]->nick = nick;
    clients[new_client_id]->nick = calloc(MAX_NICK, sizeof(char));
    strcpy(clients[new_client_id]->nick, nick);
    clients[new_client_id]->socket_id = client_fd;
    clients[new_client_id]->active = true;

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = client_fd;

    clients[new_client_id]->event_data = calloc(1, sizeof(Event_Data));
    clients[new_client_id]->event_data->event_type = CLIENT;
    clients[new_client_id]->event_data->socket_fd = client_fd;
    event.data.ptr = clients[new_client_id]->event_data;

    epoll_ctl(EPOLL_FD, EPOLL_CTL_ADD, client_fd, &event);

    return true;
}

void remove_client(int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i]->socket_id == client_fd)
        {
            free(clients[i]->nick);
            clients[i]->socket_id = -1;
            clients[i]->active = false;
            free(clients[i]->event_data);

            close(client_fd);
            return;
        }
    }
    return;
}

void list_users(int client_fd)
{
    char active_users[512] = "";

    printf("LISTING\n");

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i]->socket_id != -1)
        {
            strcat(active_users, clients[i]->nick);
            strcat(active_users, " ");
        }
    }

    printf("%s\n", active_users);

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

    web_event_data = calloc(1, sizeof(Event_Data));
    web_event_data->event_type = SOCKET;
    web_event_data->socket_fd = WEB_SOCKET_FD;
    event.data.ptr = web_event_data;

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

    unix_event_data = calloc(1, sizeof(Event_Data));
    unix_event_data->event_type = SOCKET;
    unix_event_data->socket_fd = UNIX_SOCKET_FD;
    event.data.ptr = unix_event_data;

    epoll_ctl(EPOLL_FD, EPOLL_CTL_ADD, UNIX_SOCKET_FD, &event);
}

void handle_stop()
{
    running = false;
    pthread_join(connection_thread, NULL);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i]->socket_id != -1)
        {
            send(clients[i]->socket_id, SERVER_STOPPED, strlen(SERVER_STOPPED), 0);
            free(clients[i]->event_data);
            close(clients[i]->socket_id);
        }
    }

    free(web_event_data);
    free(unix_event_data);

    close(WEB_SOCKET_FD);
    close(UNIX_SOCKET_FD);

    fclose(log_file);

    exit(0);
}

void *connections_handler()
{
    struct epoll_event events[MAX_CLIENTS];

    while (running)
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
                recv(client_fd, (char *)nick, MAX_NICK, 0);

                if (!add_client(nick, client_fd))
                {
                    send(client_fd, FULL, strlen(FULL), 0);
                    close(client_fd);

                    continue;
                }

                send(client_fd, CONNECTED, strlen(CONNECTED), 0);

                free(nick);
            }
            else if (event_data->event_type == CLIENT)
            {
                char *msg = calloc(MAX_MSG, sizeof(char));
                recv(event_data->socket_fd, (char *)msg, MAX_MSG, 0);

                printf("GOT: [%d] %s\n", event_data->socket_fd, msg);

                if (strlen(msg) == 0)
                {
                    continue;
                }

                perform_log(msg);

                char *command = strtok(msg, DELIMITER);

                if (strcmp(command, PING) == 0)
                {
                    refresh_user(event_data->socket_fd);
                }

                if (strcmp(command, LIST) == 0)
                {
                    list_users(event_data->socket_fd);
                }

                if (strcmp(command, STOP) == 0)
                {
                    pthread_mutex_lock(&mutex);
                    remove_client(event_data->socket_fd);
                    pthread_mutex_unlock(&mutex);
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

                free(msg);
            }
        }
    }

    return NULL;
}

void refresh_user(int client_fd)
{
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i]->socket_id == client_fd)
        {
            clients[i]->active = true;
            return;
        }
    }
}

void *ping()
{
    while (running)
    {
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            if (clients[i]->socket_id == -1)
            {
                continue;
            }

            clients[i]->active = false;

            send(clients[i]->socket_id, PING, strlen(PING), 0);

            sleep(PING_TIMEOUT);

            if (!clients[i]->active && clients[i]->socket_id != -1)
            {
                send(clients[i]->socket_id, CLIENT_REMOVED, strlen(CLIENT_REMOVED), 0);

                pthread_mutex_lock(&mutex);
                remove_client(clients[i]->socket_id);
                pthread_mutex_unlock(&mutex);
            }
        }
    }

    return NULL;
}

void perform_log(char *message)
{
    char log[2 * MAX_MSG];
    char date[64];

    time_t rawtime;
    time(&rawtime);

    struct tm *timeinfo = localtime(&rawtime);

    sprintf(date, "%s", asctime(timeinfo));
    date[strlen(date) - 1] = '\0'; // Remove \n

    sprintf(log, "[%s] %s\n", date, message);
    fwrite(log, sizeof(char), strlen(log), log_file);
}