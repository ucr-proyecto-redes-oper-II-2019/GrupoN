#include <thread>
#include "tcplite.h"
#include "Request.h"
#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <omp.h>
#include <signal.h>
#include <iostream>

//#define SEM_NAME "/mutex_envi5"
//#define SEM_NAME2 "/mutex_recvi5"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
using namespace std;

struct memory {
    request reqst;
    int lleno;
};
static memory* envio;
static memory* recibo;

sem_t * mutex_env;
sem_t * mutex_recv;
char * SEM_NAME;
char * SEM_NAME2;
int shmid1;
int shmid2;
TCPLite tcpl;

memory * make_shm(int key, int * shmid){
    // shared memory create
    *shmid = shmget(key, sizeof(memory), IPC_CREAT | 0666);
    // shared memory error check
    if (*shmid < 0){
        perror ("shmget\n");
        exit (EXIT_FAILURE);
    }
    return (memory*)shmat(*shmid, nullptr, 0);
}

void copy(char * v1, char * v2, int size){
    for (int i = 0; i < size; ++i){
        v1[i] = v2[i];
    }

}

void intHandler(int senal) {
    if(senal == SIGINT ){
        cout<<"catched en tcpl\n";
        tcpl.closeSocket();
        sem_unlink (SEM_NAME);
        sem_close(mutex_env);
        sem_unlink (SEM_NAME2);
        sem_close(mutex_recv);
        /* shared memory detach */
        shmdt(envio);
        shmctl(shmid1, IPC_RMID, nullptr);
        shmdt(recibo);
        shmctl(shmid2, IPC_RMID, nullptr);
            exit(0);
    }
    exit(1);
}

int main(int argc, char * argv[]){
    /* MEMORIA COMPARTIDA DE ENVIO */
    
    envio = make_shm(atoi(argv[1]), &shmid1);

    /* MEMORIA COMPARTIDA DE RECIBO */
   
    recibo = make_shm(atoi(argv[2]), &shmid2);
    SEM_NAME = argv[5];
    SEM_NAME2 = argv[6];


/*************************************************************/
    mutex_env = sem_open(SEM_NAME,O_RDWR);
    if (mutex_env == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    mutex_recv = sem_open(SEM_NAME2,O_RDWR);
    if (mutex_recv == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }


    tcpl.setAll(atoi(argv[3]),atoi(argv[4]));
/*************************************************************/

    #pragma omp parallel num_threads(4) shared(tcpl)
    {
        if (signal(SIGINT, intHandler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");


         if(omp_get_thread_num() == 0){
            while(1){
                #pragma omp critical (receptor)
                {
                    tcpl.receive();
                }
            }    
        }else if(omp_get_thread_num() == 1) {
            while(1){

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
                            recibo->reqst.size = req.size;
                            recibo->lleno = 1;
                        }
                        sem_post(mutex_recv);
                    }
                }
            }    
        }else if(omp_get_thread_num() == 2){
            while(1){
                #pragma omp critical (emisor)
                {
                    sem_wait(mutex_env);
                    if(envio->lleno){
                        cout<<"pone en bolsa de envio\n";
                        tcpl.send(envio->reqst.IP,envio->reqst.port,envio->reqst.paquete,envio->reqst.size);
                        envio->lleno = 0;
                    }
                    sem_post(mutex_env);
                }
            }    
        }else{
            while(1){
                #pragma omp critical (emisor)
                {
                    cout<<"envia\n";
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
    shmctl(shmid1, IPC_RMID, nullptr);
    shmdt(recibo);
    shmctl(shmid2, IPC_RMID, nullptr);

    return 0;

}
