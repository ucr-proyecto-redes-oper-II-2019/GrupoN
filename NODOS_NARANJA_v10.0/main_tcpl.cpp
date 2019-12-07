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
#include <omp.h>
#include "tcplite.h"

#define SEM_NAME "/mutex_envi5"
#define SEM_NAME2 "/mutex_recvi5"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
using namespace std;

struct memory {
    request reqst;
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


void copy(char * v1, char * v2, int size){
    for (int i = 0; i < size; ++i){
        v1[i] = v2[i];
    }

}



int main(int argc, char * argv[]){
    /* MEMORIA COMPARTIDA DE ENVIO */
    int shmid1;
    envio = make_shm(atoi(argv[1]), &shmid1);

    /* MEMORIA COMPARTIDA DE RECIBO */
    int shmid2;
    recibo = make_shm(atoi(argv[2]), &shmid2);

/*************************************************************/
    sem_t * mutex_env = sem_open(SEM_NAME,O_RDWR);
    if (mutex_env == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    sem_t * mutex_recv = sem_open(SEM_NAME2,O_RDWR);
    if (mutex_recv == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    TCPLite tcpl(20,8088);
/*************************************************************/

    #pragma omp parallel num_threads(4) shared(tcpl)
    {

        while(1){
             if(omp_get_thread_num() == 0){
                #pragma omp critical (receptor)
                {
                    tcpl.receive();
                }    

            }else if(omp_get_thread_num() == 1) {
                #pragma omp critical (receptor)
                {
                    request req;
                    int bandera = tcpl.getPaqueteRcv(&req);
                    if(bandera){
                        sem_wait(mutex_recv);
                        if(!recibo->lleno){
                            recibo->reqst.port = req.port;
                            recibo->reqst.IP = req.IP;
                            recibo->reqst.paquete = req.paquete;
                            recibo->lleno = 1;
                        }
                        sem_post(mutex_recv);
                    }
                    
                }  

            }else if(omp_get_thread_num() == 2){
                #pragma omp critical (emisor)
                {
                    sem_wait(mutex_env);
                    if(envio->lleno){
                        tcpl.send(envio->reqst.IP,envio->reqst.port,envio->reqst.paquete);
                        envio->lleno = 0;
                    }
                    sem_post(mutex_recv);

                }

            }else{
                #pragma omp critical (emisor)
                {
                    tcpl.send_timeout();
                }    


            }

        }

    }



/***********************************************************/

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
