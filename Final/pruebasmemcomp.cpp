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

  int key = 1333;
  int key2 = 5486;
  int shmid;
  int shmid2;
  envio = make_shm(key,&shmid);
  recibo = make_shm(key2,&shmid2);
  envio->lleno =0;
  char SEM_NAME[4];
  SEM_NAME[0] = 'h';
  SEM_NAME[1] = 'j';
  SEM_NAME[2] = '9';
  SEM_NAME[3] = '\0';
  char * SEM_NAME2 = "el chinamo 2";
  sem_t * mutex_memEnv = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

  if (mutex_memEnv == SEM_FAILED) {
      perror("semaforo failed");
      exit(EXIT_FAILURE);
  }

  sem_t * mutex_memRecv = sem_open(SEM_NAME2, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);

  if (mutex_memRecv == SEM_FAILED) {
      perror("semaforo failed");
      exit(EXIT_FAILURE);
  }
  char key_env[(sizeof(int)*8+1)];
  sprintf(key_env,"%d",key);
  char key_rcv[(sizeof(int)*8+1)];
  sprintf(key_rcv,"%d",key2);
  pid_t pid = fork();
  if (pid == 0) {
    //cout<<"k env: "<<key_env<<" k rcv: "<<key_rcv<<" port: "<<puerto<<" sem1: "<<SEM_NAME<<" sem2: "<<SEM_NAME2<<endl;
      char * ls_args[] = { "./prueba1",key_env,key_rcv,SEM_NAME,SEM_NAME2,NULL};
      system("g++ prueba1.cpp -std=c++11 -pthread -o prueba1 -Wno-write-strings");
      execvp(ls_args[0],ls_args);
  }else{
    for (size_t i = 0; i < 5;) {
      request req;

      sem_wait(mutex_memEnv);
      if(!envio->lleno){
          cout<<i<<endl;
          envio->lleno = 20+i;
          //req.ttl = 5;
          i++;
      }
      sem_post(mutex_memEnv);
    }
    for (size_t i = 0; i < 5;) {
      request req;
      sem_wait(mutex_memRecv);
      if(recibo->lleno){
          cout<<"recv lleno en pmc: "<<recibo->lleno<<endl;
          recibo->lleno = 0;
          //req.ttl = 5;
          i++;
      }
      sem_post(mutex_memRecv);
    }
  }

  int stat;
    waitpid(pid, &stat, 0);

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
