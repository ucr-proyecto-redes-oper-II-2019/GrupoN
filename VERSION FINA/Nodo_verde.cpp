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
#include <signal.h>

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

sem_t * mutex_memEnv;
sem_t * mutex_memRecv;
struct memory {
    struct request request;
    int lleno;
};
memory* envio;
memory* recibo;
char SEM_NAME[10];
char SEM_NAME2[15];
int shmid1;
int shmid2;
int key_envio;
int key_recibo;
pid_t pid;

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

void copiar(char * src, char * dest, int tam){
  for(int i = 0 ; i < 15 ; i++){
    dest [i] = '\0';
  }
  for (int i = 0; i < tam; i++) {
    dest[i] = src[i];
  }
}

void clear(char * vector,int size){
  for (int i = 0; i < size; i++) {
    vector[i] = '\0';
  }
}

void send(char * IP, int port, char * req_paquete, int tam){
  int puso_paquete = 0;
  while(!puso_paquete){
    sem_wait(mutex_memEnv);
    if(!envio->lleno){
      copiar(IP,envio->request.IP,strlen(IP));
      envio->request.port = port;
      copiar(req_paquete,envio->request.paquete,tam);
      envio->request.size = tam;
      envio->lleno = 1;
      puso_paquete = 1;
      //cout<<"envio lleno en send de naranja: "<<envio->lleno<<endl;
    }
    sem_post(mutex_memEnv);
  }


}


void randstring(char randomString[],int length) {

    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    for (int n = 0;n < length;n++) {
        int key = rand() % (int)(sizeof(charset) -1);
        randomString[n] = charset[key];
    }

    randomString[length] = '\0';

}


void intHandler(int senal) {
    if(senal == SIGINT ){

        int stat;
        waitpid(pid, &stat, 0);
        sem_unlink (SEM_NAME);
        sem_close(mutex_memEnv);
        sem_unlink (SEM_NAME2);
        sem_close(mutex_memRecv);
        /* shared memory detach */
        shmdt(envio);
        shmctl(shmid1, IPC_RMID, nullptr);
        shmdt(recibo);
        shmctl(shmid2, IPC_RMID, nullptr);
        cout << "Nodo verde terminado existosamente" << endl;
        exit(0);
    }
    exit(1);
}

//ARGUMENTOS
//argv[1] = IP del naranja al que quiere conectarse
//argv[2] = puerto del naranja al que quiere conectarse
int main(int argc, char * argv[]){
	//cada naranja tiene que tener semaforos unicos, no puede tener el mismo a otro naranja, cuando se llama al proceso de tcpl este si lo tienen que compartir entonces el nombre se pasa por param
	  srand(time(0));
 	  randstring(SEM_NAME,10);
    SEM_NAME[0] = '/';
 	  randstring(SEM_NAME2,15);
    SEM_NAME2[0] = '/';

    /* MEMORIA COMPARTIDA DE ENVIO */
    key_envio = rand()+1;
    key_recibo = rand()+1;
    envio = make_shm(key_envio, &shmid1);
    /* MEMORIA COMPARTIDA DE RECIBO */
    recibo = make_shm(key_recibo, &shmid2);

    envio->lleno = 0;
    recibo->lleno = 0;

/*************************************************************/
    mutex_memEnv = sem_open(SEM_NAME, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (mutex_memEnv == SEM_FAILED) {
        perror("semaforo 1 verde failed");
        exit(EXIT_FAILURE);
    }

    mutex_memRecv = sem_open(SEM_NAME2, O_CREAT | O_EXCL, SEM_PERMS, INITIAL_VALUE);
    if (mutex_memRecv == SEM_FAILED) {
        perror("semaforo 2 verde failed");
        exit(EXIT_FAILURE);
    }

  //  cout << "esto es recibo->lleno1 " << recibo->lleno<<endl;

    N_verde verde(argv[3],atoi(argv[4]));
    char puerto[(sizeof(int)*8+1)];
    sprintf(puerto,"%d",verde.getPuerto());


    char key_env[(sizeof(int)*8+1)];
    sprintf(key_env,"%d",key_envio);

    char key_rcv[(sizeof(int)*8+1)];
    sprintf(key_rcv,"%d",key_recibo);

    cout << "Nodo verde inicializado" << endl;
    pid = fork();
    if (pid == 0) {
       char * ls_args[] = { "./tcpl2",key_rcv,key_env,"20",puerto,SEM_NAME,SEM_NAME2,NULL};
       system("g++ tcplite.cpp main_tcpl.cpp bolsa.cpp -pthread -fopenmp -Wno-write-strings -std=c++11 -o tcpl2");
       execvp(ls_args[0],ls_args);

    }else{
	     sleep(5);
       vector<request> cola_de_FileExists;
       vector<request> cola_de_FileComplete;
       vector<request> cola_de_LocateFile;
       vector<request> cola_de_RemoveFile;
       vector<request> cola_de_PutFile;
       vector<request> cola_de_GetFile;
       vector<request> cola_de_Exec;
       vector<request> cola_de_ExecStop;
       vector<request> cola_de_GreetN;
       vector<request> cola_de_ConnectACK; //se van a recibir varios paquetes con esta solicitud por lo que se necesita una cola
   		 char * IPNaranja = argv[1];
   		 int puertoNaranja = atoi(argv[2]);
       char paquete[15];
    	 verde.connect(paquete);
	     char numero[4];
       numero[3] = '\0';
	     numero[2] = '\0';
	     numero[1] = '\0';
	     numero[0] = paquete[6];
	     int * solicitud = reinterpret_cast<int*>(&numero);
  		 send(IPNaranja,puertoNaranja,paquete,15);

      //cout << "esto es recibo->lleno3 " << recibo->lleno<<endl;
		#pragma omp parallel num_threads(4) shared(mutex_memRecv,mutex_memEnv,cola_de_FileExists,cola_de_FileComplete,cola_de_LocateFile,\
		cola_de_RemoveFile,cola_de_PutFile,cola_de_GetFile,cola_de_ExecStop,cola_de_Exec,cola_de_ConnectACK)
		{
      if (signal(SIGINT, intHandler) == SIG_ERR)
          printf("\ncan't catch SIGINT\n");

			int hilo = omp_get_thread_num();

			if(hilo==0){
                while(1){
                    request req;
                    //cout << "esto es recibo->lleno4 " << recibo->lleno<<endl;
                    sem_wait(mutex_memRecv);

                    if(recibo->lleno){
                        req.port = recibo->request.port;
                        copiar(recibo->request.IP,req.IP,strlen(recibo->request.IP));
                        copiar(recibo->request.paquete,req.paquete,recibo->request.size);
                        req.size = recibo->request.size;
                        recibo -> request.port = 0;
                        clear(recibo->request.IP,strlen(recibo->request.IP));
                        clear(recibo->request.paquete,recibo->request.size);
                        recibo -> request.size = 0;
                        recibo->lleno = 0;
                        char numero[4];
                        numero[3] = '\0';
                        numero[2] = '\0';
                        numero[1] = '\0';
                        numero[0] = req.paquete[6];
                        int * solicitud = reinterpret_cast<int*>(&numero);
                        cout<<"Solicitud: "<<*solicitud<<endl;
                        switch(*solicitud){
                            case CONNECT_ACK:
                            cola_de_ConnectACK.push_back(req);
                            cout << "Me llego un connect ack" <<endl;
                            break;
                            case GREET_NEIGHBOR:
                            cola_de_GreetN.push_back(req);
                            cout << "Me llego un hello" << endl;
                            break;
                        }
                    }
                    sem_post(mutex_memRecv);
                }
			}else if(hilo == 1){
                while(1){
                    for(int i = 0; i < cola_de_ConnectACK.size();++i){
                        char id_vecino[4];
                        id_vecino[0] = cola_de_ConnectACK[i].paquete[16];
                        id_vecino[1] = cola_de_ConnectACK[i].paquete[15];
                        id_vecino[2] = '\0';
                        id_vecino[3] = '\0';
                        int * num_id_vecino= (int*)(&id_vecino);
                        int numIDVecino = *num_id_vecino;
                        verde.llenarDatos(cola_de_ConnectACK[i].paquete,cola_de_ConnectACK[i].size);
                        char hello[15];
                        for(int j = 0; j < verde.getVecinos().size(); ++j ){
                          verde.greet_neighbor(hello);
                          if(cola_de_ConnectACK[j].size > 17){
                            send(verde.getVecinos()[j].IP, verde.getVecinos()[j].puerto, hello, 15);
                          }
                        }
                        cola_de_ConnectACK.erase(cola_de_ConnectACK.begin()+i);
                    }
                }
            }else if(hilo == 2){
                while (1) {
                    for (int i = 0; i < verde.getVecinos().size(); i++) {
                        for (int j = 0; j < verde.getVecinos().size(); j++) {
                            if (i!=j) {
                                if (verde.getVecinos()[i].puerto > -1) {
                                    char pack[21];
                                    verde.send_route(pack, verde.getVecinos()[j]);
                                    send(verde.getVecinos()[i].IP,verde.getVecinos()[i].puerto, pack, 21);
                                }
                            }
                        }
                    }
                    sleep(1);
                }
            }else if(hilo == 3){
              while(1){
                  for(int j = 0; j < cola_de_GreetN.size(); j++){
                    char id_vecino[4];
                    id_vecino[0] = cola_de_GreetN[j].paquete[5];
                    id_vecino[1] = cola_de_GreetN[j].paquete[4];
                    id_vecino[2] = '\0';
                    id_vecino[3] = '\0';
                    int * num_id_vecino= (int*)(&id_vecino);
                    int numIDVecino = *num_id_vecino;
                    cout << "Voy a anadir vecino " << numIDVecino << " con IP:" <<  cola_de_GreetN[j].IP << ":" << cola_de_GreetN[j].port<<endl;
                    verde.setVecino(numIDVecino, cola_de_GreetN[j].IP,  cola_de_GreetN[j].port);
                    cout << "Vecino anadido " << numIDVecino <<endl;
                    cola_de_GreetN.erase(cola_de_GreetN.begin()+j);
                  }
            }
          }
	  	}
    }//else del fork

    int stat;
    waitpid(pid, &stat, 0);

    return 0;

}
