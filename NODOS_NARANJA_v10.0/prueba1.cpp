#include<stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "Request.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <bits/stdc++.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <string.h>
using namespace std;
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
struct memory {
    struct request request;
    int lleno;
};
memory* envio;
memory* recibo;
memory * make_shm(int key, int * shmid){
    // key value of shared memory
    int key1 = key;

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

  int key = atoi(argv[1]);
  int shmid;
  recibo = make_shm(key,&shmid);
  int key2 = atoi(argv[2]);
  int shmid2;
  envio = make_shm(key2,&shmid2);

  char * SEM_NAME = argv[3];
  sem_t * mutex_memEnv = sem_open(SEM_NAME,O_RDWR);;
  if (mutex_memEnv == SEM_FAILED) {
      perror("semaforo p1  failed");
      exit(EXIT_FAILURE);
  }

  char * SEM_NAME2 = argv[4];
  sem_t * mutex_memRecv = sem_open(SEM_NAME2,O_RDWR);;
  if (mutex_memRecv == SEM_FAILED) {
      perror("semaforo p1  failed");
      exit(EXIT_FAILURE);
  }


    for (size_t i = 0; i < 5;) {
      request req;
      sem_wait(mutex_memRecv);
      if(recibo->lleno){
        cout<<"recv lleno p1: "<<recibo->lleno<<endl;
        recibo->lleno =0;
        ++i;

      }
      sem_post(mutex_memRecv);

    }


    for (size_t i = 0; i < 5;) {
      sem_wait(mutex_memEnv);
      if(!envio->lleno){
          envio->lleno = 10+i;
          //req.ttl = 50;
          i++;
      }
      sem_post(mutex_memEnv);
    }

    /* cleanup semaphores */
    sem_unlink (SEM_NAME);
    sem_close(mutex_memEnv);
    sem_unlink (SEM_NAME2);
    sem_close(mutex_memRecv);
    /* shared memory detach */
    shmdt(envio);
    shmctl(shmid, IPC_RMID, 0);
    shmdt(recibo);
    shmctl(shmid2, IPC_RMID, 0);
  return 0;
}
