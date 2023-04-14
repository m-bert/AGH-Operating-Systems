#include "common.h"

int main(int argc, char *argv[])
{
    const key_t server_queue_key = ftok(HOME_PATH, SERVER_KEY_SEED);
    const int server_queue_id = msgget(server_queue_key, IPC_CREAT);

    return 0;
}
