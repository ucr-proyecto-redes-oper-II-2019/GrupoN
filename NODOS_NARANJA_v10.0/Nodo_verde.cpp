#include<stdio.h>
#include <stdlib.h>
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
#include <bits/stdc++.h>
#include <sys/wait.h>
#include "n_verde.h"

#define REQMAXSIZE 1020
#define PACKETSIZE 1015

//SOLICITUDES
#define GREET_NEIGHBOR 100
#define FILE_EXISTS 102
#define FILE_COMPLETE 104
#define LOCATE_FILE 106
#define REMOVE_FILE 108
#define PUT_FILE 110
#define GET_FILE 112
#define EXEC 114
#define EXEC_STOP 116
#define CONNECT_ACK 201

////////////
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1
using namespace std;

sem_t * mutex_env;
sem_t * mutex_recv;
struct memory {
    struct request request;
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

void send(char * IP, int port, char * req_paquete){
	sem_wait(mutex_env);
	    if(!envio->lleno){
	    	envio->request.IP = IP;
	    	envio->request.port = port;
	    	envio->request.paquete = req_paquete;
	        envio->lleno = 1;
	    }
	sem_post(mutex_recv);

}

void randstring(char randomString[],int length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";        

    for (int n = 0;n < length;n++) {            
        int key = rand() % (int)(sizeof(charset) -1);
        randomString[n] = charset[key];
    }

    randomString[length] = '\0';
    
}


//ARGUMENTOS
//argv[1] = IP del naranja al que quiere conectarse 
//argv[2] = puerto del naranja al que quiere conectarse 
int main(int argc, char * argv[]){
	//cada naranja tiene que tener semaforos unicos, no puede tener el mismo a otro naranja, cuando se llama al proceso de tcpl este si lo tienen que compartir entonces el nombre se pasa por param
	char SEM_NAME[10];
	randstring(SEM_NAME,10);
	char SEM_NAME2[10];
	randstring(SEM_NAME2,10);

	/* MEMORIA COMPARTIDA DE ENVIO */
    int shmid1;
    int key_envio = rand()%9000;
    int key_recibo = rand()%9000;

    envio = make_shm(key_envio, &shmid1);

    /* MEMORIA COMPARTIDA DE RECIBO */
    int shmid2;
    recibo = make_shm(key_recibo, &shmid2);

/*************************************************************/
    mutex_env = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (mutex_env == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    mutex_recv = sem_open(SEM_NAME2, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (mutex_recv == SEM_FAILED) {
        perror("sem_open(3) failed");
        exit(EXIT_FAILURE);
    }

    N_verde verde;
    char puerto[(sizeof(int)*8+1)];
    sprintf(puerto,"%d",verde.getPuerto());
   

    char key_env[(sizeof(int)*8+1)];
    sprintf(key_env,"%d",key_envio);

    char key_rcv[(sizeof(int)*8+1)];
    sprintf(key_rcv,"%d",key_recibo);

    pid_t pid = fork();
    if (pid == 0) {
    
        char * ls_args[] = { "./tcpl",key_env,key_rcv,"20",puerto,SEM_NAME,SEM_NAME2,NULL};
        system("g++ main_tcpl.cpp -o tcpl -pthread -std=c++11");
        execvp(ls_args[0],ls_args);

    }else{

    	vector<request> cola_de_FileExists;
		vector<request> cola_de_FileComplete;
	  	vector<request> cola_de_LocateFile;
		vector<request> cola_de_RemoveFile;
		vector<request> cola_de_PutFile;
		vector<request> cola_de_GetFile;
	  	vector<request> cola_de_Exec;
	  	vector<request> cola_de_ExecStop;
    	vector<request> cola_de_ConnectACK; //se van a recibir varios paquetes con esta solicitud por lo que se necesita una cola

 		char * IPNaranja = argv[1];
 		int puertoNaranja = atoi(argv[2]);
    	char paquete[PACKETSIZE];
		verde.connect(paquete);
		send(IPNaranja,puertoNaranja,paquete);    	



		
		



		#pragma omp parallel num_threads(6) shared(mutex_recv,mutex_env,cola_de_FileExists,cola_de_FileComplete,cola_de_LocateFile,\
		cola_de_RemoveFile,cola_de_PutFile,cola_de_GetFile,cola_de_ExecStop,cola_de_Exec,cola_de_ConnectACK)
		{

			int hilo = omp_get_thread_num();

			if(hilo==0){

				request req;

				sem_wait(mutex_recv);
				if(recibo->lleno){
					req.port = recibo->request.port;
					req.IP = recibo->request.IP;
					req.paquete = recibo->request.paquete;
					recibo->lleno = 0;

				}
				sem_post(mutex_recv);

				int * soliciud = reinterpret_cast<int*>(&req.paquete[6]);

				switch(*soliciud){
					case CONNECT_ACK: cola_de_ConnectACK.push_back(req);
					break;

				}


			}else if(hilo == 1){
				while(1){
					#pragma omp critical(emisor)
					{
						for(int i = 0; i < cola_de_ConnectACK.size();++i){

							verde.llenarDatos(cola_de_ConnectACK[i].paquete);

						}

					}
				}


			}



	  	}

	  


    }//else del fork


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