#include "common.h"

int hairdressers_no, chairs_no, waiting_room_seats;

int hairdressers_shm_desc, chairs_shm_desc, waiting_shm_desc;
int *hairdressers_shm, *chairs_shm, *waiting_shm;

sem_t *hairdressers_sem, *chairs_sem, *waiting_sem;

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
            sem_trywait(waiting_sem);

            // No more space in waiting room, client leaves
            if (errno == EAGAIN)
            {
                printf("[%d] leaves - no room\n", getpid());
                exit(0);
            }

            pid_t client_id = getpid();

            // Take a seat in waiting room
            int waiting_id = find_index(waiting_shm, waiting_room_seats);
            waiting_shm[waiting_id] = client_id;

            // Wait for hairdresser
            sem_wait(hairdressers_sem);
            printf("[%d] found hairdresser\n", client_id);

            // Hairdresser found
            int hairdresser_id = find_index(hairdressers_shm, hairdressers_no);
            hairdressers_shm[hairdresser_id] = client_id;

            // Wait for free chair to get haircut
            sem_wait(chairs_sem);
            printf("[%d] found chair\n", client_id);

            // Free seat in waiting room
            sem_post(waiting_sem);
            waiting_shm[waiting_id] = -1;
            printf("[%d] Freeing waiting seat\n", client_id);

            // Getting haircut
            int chair_id = find_index(chairs_shm, chairs_no);
            chairs_shm[chair_id] = client_id;

            int haircut_time = get_haircut_time();
            sleep(haircut_time);

            // Free chair and hairdresser
            sem_post(chairs_sem);
            chairs_shm[chair_id] = -1;
            printf("[%d] Freeing chair\n", client_id);

            sem_post(hairdressers_sem);
            hairdressers_shm[hairdresser_id] = -1;
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
    hairdressers_shm_desc = shm_open(HAIRDRESSER_SHM, O_RDWR | O_CREAT, 0666);
    ftruncate(hairdressers_shm_desc, hairdressers_no * sizeof(int));
    hairdressers_shm = (int *)mmap(NULL, hairdressers_no * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, hairdressers_shm_desc, 0);
    init_array(hairdressers_shm, hairdressers_no);

    chairs_shm_desc = shm_open(CHAIRS_SHM, O_RDWR | O_CREAT, 0666);
    ftruncate(chairs_shm_desc, chairs_no * sizeof(int));
    chairs_shm = (int *)mmap(NULL, chairs_no * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, chairs_shm_desc, 0);
    init_array(chairs_shm, chairs_no);

    waiting_shm_desc = shm_open(WAITING_SHM, O_RDWR | O_CREAT, 0666);
    ftruncate(waiting_shm_desc, waiting_room_seats * sizeof(int));
    waiting_shm = (int *)mmap(NULL, waiting_room_seats * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, waiting_shm_desc, 0);
    init_array(waiting_shm, waiting_room_seats);
}

void remove_shm()
{
    munmap(hairdressers_shm, hairdressers_no * sizeof(int));
    munmap(chairs_shm, chairs_no * sizeof(int));
    munmap(waiting_shm, waiting_room_seats * sizeof(int));

    shm_unlink(HAIRDRESSER_SHM);
    shm_unlink(CHAIRS_SHM);
    shm_unlink(WAITING_SHM);
}

void create_sem()
{
    hairdressers_sem = sem_open(HAIRDRESSER_SEM, O_RDWR | O_CREAT, 0666, hairdressers_no);
    sem_init(hairdressers_sem, SHARE, hairdressers_no);

    chairs_sem = sem_open(CHAIRS_SEM, O_RDWR | O_CREAT, 0666, chairs_no);
    sem_init(chairs_sem, SHARE, chairs_no);

    waiting_sem = sem_open(WAITING_SEM, O_RDWR | O_CREAT, 0666, waiting_room_seats);
    sem_init(waiting_sem, SHARE, waiting_room_seats);
}

void remove_sem()
{
    sem_close(hairdressers_sem);
    sem_close(chairs_sem);
    sem_close(waiting_sem);
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