#include "common.h"

int hairdressers_sm_id, chairs_sm_id, waiting_sm_id;
int hairdressers_no, chairs_no, waiting_room_seats;

int *hairdressers_sm, *chairs_sm, *waiting_sm;

int semaphores_id;

bool spawning = true;

void validate_parameters(int argc, char *argv[]);

void create_shm();
void remove_shm();

void create_sem();
void remove_sem();

void stop_program();

int main(int argc, char *argv[])
{
    validate_parameters(argc, argv);
    srand(time(NULL));

    struct sigaction action;
    action.sa_handler = &stop_program;
    sigaction(SIGINT, &action, NULL);

    hairdressers_no = atoi(argv[1]);
    chairs_no = atoi(argv[2]);
    waiting_room_seats = atoi(argv[3]);

    create_shm();
    create_sem();

    while (spawning)
    {
        int new_process_pid = fork();

        switch (new_process_pid)
        {
        case -1: // Fork failed
            break;

        case 0: // Child
            printf("[%d] Enters\n", getpid());
            // Check if there is a free chair in waiting room
            decrement_sem(semaphores_id, WAITING_SEM, NO_WAIT);

            // No more space in waiting room, client leaves
            if (errno == EAGAIN)
            {
                printf("[%d] leaves - no room\n", getpid());
                exit(0);
            }

            pid_t client_id = getpid();

            // Take a seat in waiting room
            int waiting_id = find_index(waiting_sm, waiting_room_seats);
            waiting_sm[waiting_id] = client_id;

            // Wait for hairdresser
            decrement_sem(semaphores_id, HAIRDRESSER_SEM, WAIT);
            printf("[%d] found hairdresser\n", client_id);

            // Hairdresser found
            int hairdresser_id = find_index(hairdressers_sm, hairdressers_no);
            hairdressers_sm[hairdresser_id] = client_id;

            // Wait for free chair to get haircut
            decrement_sem(semaphores_id, CHAIRS_SEM, WAIT);
            printf("[%d] found chair\n", client_id);

            // Free seat in waiting room
            increment_sem(semaphores_id, WAITING_SEM);
            waiting_sm[waiting_id] = -1;
            printf("[%d] Freeing waiting seat\n", client_id);

            // Getting haircut
            int chair_id = find_index(chairs_sm, chairs_no);
            chairs_sm[chair_id] = client_id;

            int haircut_time = get_haircut_time();
            sleep(haircut_time);

            // Free chair and hairdresser
            increment_sem(semaphores_id, CHAIRS_SEM);
            chairs_sm[chair_id] = -1;
            printf("[%d] Freeing chair\n", client_id);

            increment_sem(semaphores_id, HAIRDRESSER_SEM);
            hairdressers_sm[hairdresser_id] = -1;
            printf("[%d] Freeing hairdresser\n", client_id);

            exit(0);

        default: // Parent
            sleep(2);
            break;
        }
    }

    remove_sem();
    remove_shm();

    return 0;
}

void validate_parameters(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Too few parameters\n");
        exit(-1);
    }

    int hd_no = atoi(argv[1]);
    int ch_no = atoi(argv[2]);
    int wrs_no = atoi(argv[3]);

    if (hd_no <= 0 || wrs_no <= 0 || ch_no <= 0)
    {
        printf("Arguments cannot be less than or equal to 0\n");
        exit(-1);
    }

    if (hd_no < ch_no)
    {
        printf("Hairdressers number must be greather than or equal to chairs number\n");
        exit(-1);
    }
}

void create_shm()
{
    hairdressers_sm_id = shmget(HAIRDRESSERS_KEY, hairdressers_no, IPC_CREAT | 0666);
    hairdressers_sm = shmat(hairdressers_sm_id, NULL, 0);
    init_array(hairdressers_sm, hairdressers_no);

    chairs_sm_id = shmget(CHAIRS_KEY, chairs_no, IPC_CREAT | 0666);
    chairs_sm = shmat(chairs_sm_id, NULL, 0);
    init_array(chairs_sm, chairs_no);

    waiting_sm_id = shmget(WAITING_KEY, waiting_room_seats, IPC_CREAT | 0666);
    waiting_sm = shmat(waiting_sm_id, NULL, 0);
    init_array(waiting_sm, waiting_room_seats);
}

void remove_shm()
{
    shmctl(hairdressers_sm_id, IPC_RMID, NULL);
    shmctl(chairs_sm_id, IPC_RMID, NULL);
    shmctl(waiting_sm_id, IPC_RMID, NULL);
}

void create_sem()
{
    semaphores_id = semget(SEMAPHORES_KEY, SEM_NO, IPC_CREAT | 0666);

    set_sem_value(semaphores_id, HAIRDRESSER_SEM, hairdressers_no);
    set_sem_value(semaphores_id, CHAIRS_SEM, chairs_no);
    set_sem_value(semaphores_id, WAITING_SEM, waiting_room_seats);
}

void remove_sem()
{
    semctl(semaphores_id, 0, IPC_RMID, NULL);
}

void stop_program()
{
    spawning = false;
    while (wait(NULL) > 0)
    {
    }

    remove_sem();
    remove_shm();

    exit(0);
}