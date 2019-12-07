#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "tcplite.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/wait.h>

#define SEM_NAME "/mutex_envi5"
#define SEM_NAME2 "/mutex_recvi5"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
using namespace std;

struct memory {
    request req;
    int lleno;
};
memory* envio;
memory* recibo;

memory * make_shm(int key, int * shmid){
    // key value of shared memory
    int key1 = 12345;
    // shared memory create
    *shmid = shmget(key1, sizeof(memory), IPC_CREAT | 0666);
    // shared memory error check
    if (*shmid < 0){
        perror ("shmget\n");
        exit (EXIT_FAILURE);
    }
    return (memory*)shmat(*shmid, NULL, 0);
}

int main(int argc, char * argv[]){
    /* MEMORIA COMPARTIDA DE ENVIO */
    int shmid1;
    envio = make_shm(1234, &shmid1);

    /* MEMORIA COMPARTIDA DE RECIBO */
    int shmid2;
    recibo = make_shm(1234, &shmid2);

/*************************************************************/
    sem_t * mutex_env = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (mutex_env == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    sem_t * mutex_recv = sem_open(SEM_NAME2, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (mutex_recv == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid == 0) {
        char * ls_args[] = { "./tcpl", "666","777",NULL};
        system("g++ main_tcpl.cpp -o tcpl -pthread");
        execvp(ls_args[0],ls_args);
    }




/***********************************************************/
    int stat;
    waitpid(pid, &stat, 0);

    /* cleanup semaphores */
    sem_unlink (SEM_NAME);
    sem_close(mutex_env);
    sem_unlink (SEM_NAME2);
    sem_close(mutex_recv);
    /* shared memory detach */
    shmdt(envio);
    shmctl(shmid1, IPC_RMID, 0);
    shmdt(recibo);
    shmctl(shmid2, IPC_RMID, 0);

    return 0;

}
