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

//#define SEM_NAME "/mutex_memEnvi5"
//#define SEM_NAME2 "/mutex_memRecvi5"
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
using namespace std;

struct memory {
    request reqst;
    int lleno;
};

memory* envio;
memory* recibo;

sem_t * mutex_memEnv;
sem_t * mutex_memRecv;
char * SEM_NAME;
char * SEM_NAME2;
int shmid1;
int shmid2;
TCPLite tcpl;

memory * make_shm(int key, int * shmid){
    // shared memory create
    *shmid = shmget(key, sizeof(memory), 0666|IPC_CREAT );
    // shared memory error check
    if (*shmid < 0){
        perror ("shmget\n");
        exit (EXIT_FAILURE);
    }
    return (memory*)shmat(*shmid, (void*)0, 0);
}

void copiar(char * dest, char * v2, int size){
    for(int i = 0 ; i < 15 ; i++){
        dest [i] = '\0';
    }
    for (int i = 0; i < size; ++i){
        dest[i] = v2[i];
    }

}

void intHandler(int senal) {
    if(senal == SIGINT ){
        cout<<"catch en tcpl\n";
        tcpl.closeSocket();
        //sem_unlink (SEM_NAME);
        sem_close(mutex_memEnv);
        //sem_unlink (SEM_NAME2);
        sem_close(mutex_memRecv);
        /* shared memory detach */
        shmdt(envio);
        //shmctl(shmid1, IPC_RMID, nullptr);
        shmdt(recibo);
        //shmctl(shmid2, IPC_RMID, nullptr);
        exit(0);
    }
    exit(1);
}

void clear(char * vector,int size){
  for (int i = 0; i < size; i++) {
    vector[i] = '\0';
  }
}

int main(int argc, char * argv[]){
    /* MEMORIA COMPARTIDA DE ENVIO */
    srand(time(0));
    envio = make_shm(atoi(argv[1]), &shmid1);
    cout << "key de envio en tcpl" <<argv[1] << endl;
    /* MEMORIA COMPARTIDA DE RECIBO */

    recibo = make_shm(atoi(argv[2]), &shmid2);
    cout << "key de recibo en tcpl" <<argv[2] << endl;
    SEM_NAME = argv[6];
    SEM_NAME2 = argv[5];


    printf("semaforo memEnv en tcpl %s\n",SEM_NAME);
    printf("semaforo memRecv en tcpl %s\n",SEM_NAME2);
/*************************************************************/
    mutex_memEnv = sem_open(SEM_NAME,O_RDWR);
    if (mutex_memEnv == SEM_FAILED) {
        perror("mutex_memEnv failed");
        exit(EXIT_FAILURE);
    }

    mutex_memRecv = sem_open(SEM_NAME2,O_RDWR);
    if (mutex_memRecv == SEM_FAILED) {
        perror("mutex_memRecv failed");
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
                //#pragma omp critical (receptor)
                //{
                    //sem_wait(tcpl.mutex_memEnv);
                    tcpl.receive();
                  //  sem_wait(tcpl.mutex_memEnv);
                //}
            }
        }else if(omp_get_thread_num() == 1) {
            while(1){

                //#pragma omp critical (receptor)
                //{
                    request req;
                    int bandera = tcpl.getPaqueteRcv(&req);
                    //cout << "request sacado" << " IP: " << req.IP << "puerto: " << req.port << "paquete: " << req.paquete << endl;

                    if(bandera != -1){
                        sem_wait(mutex_memEnv);
                        if(!envio->lleno){
                            envio->reqst.port = req.port;
                            copiar(envio->reqst.IP,req.IP,strlen(req.IP));
                            copiar(envio->reqst.paquete, req.paquete,req.size);
                            envio->reqst.size = req.size;
                            envio->lleno = 1;
                            tcpl.borrar_indice_recv(bandera);
                        }
                        sem_post(mutex_memEnv);
                    }
                //}
            }
        }else if(omp_get_thread_num() == 2){
            while(1){

                  sem_wait(mutex_memRecv);

                  //cout << "envio->lleno" << envio->lleno <<endl;
                  //cout << "hilo 2 entra a zona critica, recibo->lleno: " << recibo->lleno <<endl;
                  if(recibo->lleno){
                      //cout<<"pone en bolsa de envio\n";
                      tcpl.send(recibo->reqst.IP,recibo->reqst.port,recibo->reqst.paquete,recibo->reqst.size);
                      clear(recibo->reqst.IP,strlen(recibo->reqst.IP));
                      recibo->reqst.port = 0;
                      clear(recibo->reqst.paquete,recibo->reqst.size);
                      recibo->reqst.size = 0;
                      recibo->lleno = 0;
                  }
                  sem_post(mutex_memRecv);

            }
        }else{
            while(1){
                //#pragma omp critical (emisor)
                //{
                    //sem_wait(mutex_memEnv);
                    //cout<<"envia\n";
                    tcpl.send_timeout();
                  //  sem_post(mutex_memEnv);
                //}
            }
        }

    }


/***********************************************************/

    /* cleanup semaphores */
    sem_unlink (SEM_NAME);
    sem_close(mutex_memEnv);
    sem_unlink (SEM_NAME2);
    sem_close(mutex_memRecv);
    /* shared memory detach */
    shmdt(envio);
    shmctl(shmid1, IPC_RMID, nullptr);
    shmdt(recibo);
    shmctl(shmid2, IPC_RMID, nullptr);

    return 0;

}
