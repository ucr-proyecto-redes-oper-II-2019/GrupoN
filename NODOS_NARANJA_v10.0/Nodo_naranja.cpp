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
#include <bits/stdc++.h>
#include <sys/wait.h>
#include "n_naranja.h"

#define REQMAXSIZE 1020
#define PACKETSIZE 1015
//SOLICITUDES
#define CONNECT 200
#define REQUEST_POS 205
#define REQUEST_POS_ACK 206
#define DISCONNECT 215
#define REMOVE 220
#define CONFIRM_POS 210
#define CONFIRM_POS_ACK 211
////////////
#define SEM_NAME "/mutex_envi5"
#define SEM_NAME2 "/mutex_recvi5"
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


int revisar_pos_vacio(int * arreglo_request_pos, int cantidad_naranjas){
  for(int i = 0; i < cantidad_naranjas; i++){
    if (arreglo_request_pos[i] == -1) {
      return 1; //no acepto alguno
    }/*else if(arreglo_request_pos[i] == -1){
      return -1; //no han llegado todos
    }*/
  }
  return 0;
}

int revisar_request_pos(int * arreglo_request_pos, int cantidad_naranjas){
  for(int i = 0; i < cantidad_naranjas; i++){
    if (arreglo_request_pos[i] == 0) {
      return 0; //no acepto alguno
    }/*else if(arreglo_request_pos[i] == -1){
      return -1; //no han llegado todos
    }*/
  }
  return 1;
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

/*void buscar_request_para_marcar_en_arreglo(vector<requests> cola, int num_req, int * arreglo_pos){ //cola en la que se

}*/

//main en el que yo estaba trabajando

int main(){


	/* MEMORIA COMPARTIDA DE ENVIO */
    int shmid1;
    envio = make_shm(1234, &shmid1);

    /* MEMORIA COMPARTIDA DE RECIBO */
    int shmid2;
    recibo = make_shm(1234, &shmid2);

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
    pid_t pid = fork();
    if (pid == 0) {
        char * ls_args[] = { "./tcpl", "666","777",NULL};
        system("g++ main_tcpl.cpp -o tcpl -pthread");
        execvp(ls_args[0],ls_args);
    }else{

		vector<request> cola_de_Connect;
		vector<request> cola_de_RequestPos;
	  	vector<request> cola_de_RequestPosACK;
		vector<request> cola_de_Disconnect;
		vector<request> cola_de_Remove;
		vector<request> cola_de_ConfirmPos;
	  	vector<request> cola_de_ConfirmPosACK;
	  	N_naranja naranja("grafo.csv","test.txt",(char*)"10.0.2.15");
		char paquete[PACKETSIZE];
		int port;
		char * IP;
		int bandera = 0;

		while(1){

			//4 hilos para manejar tcpl y los otros 4 son para cada solicitud
	    #pragma omp parallel num_threads(6) shared(paquete,port,IP,bandera,mutex_recv,mutex_env, cola_de_Connect,cola_de_RequestPos, cola_de_Disconnect,cola_de_Remove,cola_de_ConfirmPos)
			{
				int hilo = omp_get_thread_num();

				if(hilo == 0) {
					#pragma omp critical (receptor)
						{
							request req;
							sem_wait(mutex_recv);
							if(recibo->lleno){
								req.port = recibo->request.port;
								req.IP = recibo->request.IP;
								req.paquete = recibo->request.paquete;
								recibo->lleno = 0;

							}
							sem_post(mutex_recv);

	            			int * solicitud = reinterpret_cast<int*>(&req.paquete[6]);

							switch(*solicitud){
								case CONNECT: cola_de_Connect.push_back(req);
								break;
								case REQUEST_POS: cola_de_RequestPos.push_back(req);
								break;
								case DISCONNECT: cola_de_Disconnect.push_back(req);
								break;
								case REMOVE: cola_de_Remove.push_back(req);
								break;
								case CONFIRM_POS: cola_de_ConfirmPos.push_back(req);
								break;
	              				case REQUEST_POS_ACK: cola_de_RequestPos.push_back(req);
	              				break;
	              				case CONFIRM_POS_ACK: cola_de_ConfirmPosACK.push_back(req);
	              				break;
							}


						}

				}else if(hilo == 1 && bandera){
					#pragma omp critical(emisor)
					{
						for(int i = 0; i < cola_de_Connect.size();++i){
							char numero_request[4];
							numero_request[0] = cola_de_RequestPos[i].paquete[3];
							numero_request[1] = cola_de_RequestPos[i].paquete[2];
							numero_request[2] = cola_de_RequestPos[i].paquete[1];
							numero_request[3] = cola_de_RequestPos[i].paquete[0];
							int * numDeRequest = reinterpret_cast<int*>(numero_request);

							/*
								revisar si se puede meter a la cola de send (aun hay espacio?) o si se aun no estan instanciados todos los
								nodos del archivo del grafo (aun se pueden unir verdes?)
								para esta ultima revision se tiene que tener un contador cada vez que metamos uno, se actauliza cuando se hace confirm ack
								en caso de no poder hacerlo envia un NACK
								en caso de si poder instanciarlo
								hace un request_pos a todos los vecinos naranjas
								si algun nodo manda un request_pos_ACK negativo se manda un NACK
								si todos le confirmaron, se llama a confirm
								los confirm ACK tambien pueden hacer que se devuelva un NACK
								si todo esto se logro entonces se hace un connect_ACK
							*/
	            vector<Nodos> vecinos_naranja;
	            vecinos_naranja = naranja.getNaranjas();
	            int ID_aceptado = 0;
	            int num_ID_verde;
	            while(!ID_aceptado){
	              int num_req = rand()+ 65536 % 65535;
	              num_ID_verde = rand() + 65536 % 65535;
	              char * request_pos_paquete;
	              naranja.request_pos(request_pos_paquete,num_req,num_ID_verde);
	              for(int j = 0 ; j < vecinos_naranja.size();j++){

	              	send(vecinos_naranja[j].IP,vecinos_naranja[j].puerto,request_pos_paquete);
	             
	                //tcpl.send(vecinos_naranja[j].IP,vecinos_naranja[j].puerto,request_pos_paquete);
	                /****si usamos memoria compartida en esta parte se meten los datos en memoria compartida******/
	              }
	              int arreglo_request_pos[vecinos_naranja.size()]; //en este arreglo se va a marcar cuando llegue algun paquete
	              for(int j = 0; j < vecinos_naranja.size(); j++){ //cuando esta en -1 significa que no han llegado todos
	                arreglo_request_pos[j] = -1;
	              }

	              while(revisar_pos_vacio(arreglo_request_pos, vecinos_naranja.size())){ //mientras que el arreglo tenga espacios vacios
	                for(int j = 0; j < cola_de_RequestPosACK.size(); j++){
	                  //aca marca cuales request pos ack han llegado, hay que tener un indice designado para cada nodo naranja, hay que ver como indicar que indice le pertenece a quien
	                  //si ya es un 1/0 no se marca
	                  //se sacan de la cola
	                }
	              }
	              int resultado = revisar_request_pos(arreglo_request_pos,vecinos_naranja.size()); //revisa que todos sean 1, si hay alguno que no es uno devuelve 0
	              ID_aceptado = !resultado ? 0:1; //si algun nodo naranja no acepta se elige otro numero
	            }
	            for(int j = 0; j < vecinos_naranja.size(); j++){
	              char * confirm_pos_paquete;
	              naranja.confirm_pos(confirm_pos_paquete,num_ID_verde); //de parametros faltan el IP y el puerto, revisar n_naranja.h

	              send(vecinos_naranja[j].IP,vecinos_naranja[j].puerto,confirm_pos_paquete);
	              //tcpl.send(vecinos_naranja[j].IP,vecinos_naranja[j].puerto,confirm_pos_paquete); /****/
	              /****si usamos memoria compartida en esta parte se meten los datos en memoria compartida******/
	            }
	            int arreglo_confirm_pos[vecinos_naranja.size()];
	            for(int j = 0; j < vecinos_naranja.size(); j++){ //cuando esta en -1 significa que no han llegado todos
	              arreglo_confirm_pos[j] = -1;
	            }
	            while(revisar_pos_vacio(arreglo_confirm_pos, vecinos_naranja.size())){ //mientras que el arreglo tenga espacios vacios
	              for(int j = 0; j < cola_de_ConfirmPosACK.size(); j++){
	                //aca marca cuales request pos ack han llegado, hay que tener un indice designado para cada nodo naranja, hay que ver como indicar que indice le pertenece a quien
	                //si ya es un 1/0 no se marca
	                //se sacan de la cola
	              }
	            }
	            

	            vector<char*> ACK;
							//int num_ID = cola_de_Connect[i]; ???
	            //request reqConnect = cola_de_Connect[i];
							
	            //se manda connect ack cuando se reciban todos los confirm_pos_ACK
							naranja.connect_ACK(&ACK,cola_de_Connect[i].port,cola_de_Connect[i].IP,*numDeRequest); /*revisar parametros connect ack*/
							//porque estaba comentado?
							for (int j = 0; j < ACK.size(); j++) {
								send(cola_de_Connect[i].IP,cola_de_Connect[i].port,ACK[i]);	
									//tcpl.send(cola_de_Connect[i].IP,cola_de_Connect[i].port,ACK[i]);
							}
	            //tcpl.send(cola_de_Connect[i].IP,cola_de_Connect[i].port,ACK);
						}
					}
				}else if(hilo == 2 && bandera){
					#pragma omp critical(emisor)
					{
						for(int i = 0; i < cola_de_RequestPos.size();++i){
							
							//request_pos_ACK(char * ACK,int num_req,int num_ID, int num_prioridad);
							char paqueteACK[PACKETSIZE];

							char num_request[4];
							num_request[0] = cola_de_RequestPos[i].paquete[3];
							num_request[1] = cola_de_RequestPos[i].paquete[2];
							num_request[2] = cola_de_RequestPos[i].paquete[1];
							num_request[3] = cola_de_RequestPos[i].paquete[0];
							int * num_req = reinterpret_cast<int*>(num_request);

							char ID[2];
							ID[0] = cola_de_RequestPos[i].paquete[5];
							ID[1] = cola_de_RequestPos[i].paquete[4];
							int * nombre = reinterpret_cast<int*>(ID);	

							char prioridad[2];
							prioridad[0] = cola_de_RequestPos[i].paquete[8];
							prioridad[1] = cola_de_RequestPos[i].paquete[7];
							int * num_prioridad = reinterpret_cast<int*>(prioridad);

							naranja.request_pos_ACK(paqueteACK, *num_req, *nombre, *num_prioridad);
							send(cola_de_RequestPos[i].IP,cola_de_RequestPos[i].port,paqueteACK);
							//tcpl.send(cola_de_RequestPos[i].IP,cola_de_RequestPos[i].port,paqueteACK);
						}

					}

				}else if(hilo == 3 && bandera){
					#pragma omp critical(emisor)
					{

						for(int i = 0; i < cola_de_Disconnect.size();++i){
							
							char paqueteACK[PACKETSIZE];
							char num_request[4];
							num_request[0] = cola_de_RequestPos[i].paquete[3];
							num_request[1] = cola_de_RequestPos[i].paquete[2];
							num_request[2] = cola_de_RequestPos[i].paquete[1];
							num_request[3] = cola_de_RequestPos[i].paquete[0];
							int * num_req = reinterpret_cast<int*>(num_request);

							char ID[2];
							ID[0] = cola_de_RequestPos[i].paquete[5];
							ID[1] = cola_de_RequestPos[i].paquete[4];
							int * nombre = reinterpret_cast<int*>(ID);	
							naranja.disconnect_ACK(paqueteACK, *num_req,*nombre);
							send(cola_de_Disconnect[i].IP,cola_de_Disconnect[i].port,paqueteACK);
							//tcpl.send(cola_de_Disconnect[i].IP,cola_de_Disconnect[i].port,paqueteACK);
						}

					}

				}else if(hilo == 4 && bandera){
					#pragma omp critical(emisor)
					{

						for(int i = 0; i < cola_de_Remove.size();++i){
						
							char paqueteACK[PACKETSIZE];
							char num_request[4];
							num_request[0] = cola_de_RequestPos[i].paquete[3];
							num_request[1] = cola_de_RequestPos[i].paquete[2];
							num_request[2] = cola_de_RequestPos[i].paquete[1];
							num_request[3] = cola_de_RequestPos[i].paquete[0];
							int * num_req = reinterpret_cast<int*>(num_request);

							char ID[2];
							ID[0] = cola_de_RequestPos[i].paquete[5];
							ID[1] = cola_de_RequestPos[i].paquete[4];
							int * nombre = reinterpret_cast<int*>(ID);		

							naranja.remove_ACK(paqueteACK,*nombre,*num_req);
							send(cola_de_Remove[i].IP,cola_de_Remove[i].port,paqueteACK);
							//tcpl.send(cola_de_Remove[i].IP,cola_de_Remove[i].port,paqueteACK);
						}

					}

				}else if(hilo == 5 && bandera){
					#pragma omp critical(emisor)
					{

						for(int i = 0; i < cola_de_ConfirmPos.size();++i){
							
							char paqueteACK[PACKETSIZE];
							char num_request[4];
							num_request[0] = cola_de_RequestPos[i].paquete[3];
							num_request[1] = cola_de_RequestPos[i].paquete[2];
							num_request[2] = cola_de_RequestPos[i].paquete[1];
							num_request[3] = cola_de_RequestPos[i].paquete[0];
							int * num_req = reinterpret_cast<int*>(num_request);

							char ID[2];
							ID[0] = cola_de_RequestPos[i].paquete[5];
							ID[1] = cola_de_RequestPos[i].paquete[4];
							int * nombre = reinterpret_cast<int*>(ID);	

							naranja.confirm_pos_ACK(paqueteACK,*num_req,*nombre);
							send(cola_de_ConfirmPos[i].IP,cola_de_ConfirmPos[i].port,paqueteACK);
							//tcpl.send(cola_de_ConfirmPos[i].IP,cola_de_ConfirmPos[i].port,paqueteACK);
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