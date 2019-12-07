#include<stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mutex>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define REQMAXSIZE 1020
using namespace std;

struct memory {
    char buff[REQMAXSIZE]={0};
    mutex interfaz;
}; struct memory* shmptr;


int main(int argc, char * argv[]){
    int shmid;
    // key value of shared memory
    int key = 12345;
    // shared memory create
    shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666);
    // shared memory error check
    if (shmid < 0){
        perror ("shmget\n");
        exit (1);
    }
    // attaching the shared memory
    shmptr = (struct memory*)shmat(shmid, NULL, 0);
    /***********************************************************/
    /* semaphore "rw_mutex" for shared processes.
  * semaphore is reached using this name
  * initial value = 1 (mutex)
 common to both reader and writer processes
 functions as a mutual exclusion semaphore for the writers.
  */
   sem_t * rw_mutex = sem_open(SEM_NAME1, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
   if (rw_mutex == SEM_FAILED) {
       perror("sem_open(3) failed");
       exit(EXIT_FAILURE);
   }




    for (size_t i = 0; i < 6; i++) {
        printf("%s\n", "ESPERANDO ZONA");
        shmptr->interfaz.lock();
        printf("%s\n", "ENTRO A LA ZONA");
        shmptr->buff[0] = '1';
        shmptr->buff[1] = '2';
        shmptr->buff[2] = '3';
        shmptr->buff[3] = '5';
        shmptr->buff[4] = '\0';
        sleep(1);

        printf("%s\n", "LIBERO ZONA");
        shmptr->interfaz.unlock();
        sleep(1);
    }


    /***********************************************************/
    /* shared memory detach */
    shmdt (shmptr);
    shmctl (shmid, IPC_RMID, 0);

    return 0;

}
