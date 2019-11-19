#include <vector>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <time.h> 
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
#define tam_paq 9
//4 b , 2 b, 1 b, 2b
vector<int> vector_nombres_usados;
int ultimo_nombre_asignado; 
int ultima_prioridad_asignada;
srand (time(NULL));

int revisar_ID(int ID){
	for(int i = 0; i < vector_nombres_usados.size(); i++){
		if(vector_nombres_usados.at(i) == ID){
			return 1;
		}
	}
	return 0;
}


int request_pos(char * paquete){

	//se elige el numero de request
	int num_req = rand();
	char * r;
	r = (char *)&num_req;
	paquete[0] = r[3];
	paquete[1] = r[2];
	paquete[2] = r[1];
	paquete[3] = r[0];

	//se elige el nombre aleatorio ID
	while(1){
		int num_ID = rand() + 65536 % 65535;
		if(!revisar_ID(num_ID)){
			char * x;
			x = (char *)&num_ID;
			paquete[4] = x[1]; //revisar
			paquete[5] = x[0];
			ultimo_nombre_asignado = num_ID; //para luego responder si ese nombre fue asignado por este nodo
			break;
		}
	}
	//se elige tarea a realizar
	int num_tarea = 205;
	char * t;
	t = (char *)&num_tarea;
	paquete[6] = t[0];

	//se elige la prioridad 
	int num_prioridad = rand() + 65536 % 65535;
	char * p;
	p = (char *)&num_prioridad;
	paquete[7] = p[1];
	paquete[8] = p[0];
	ultima_prioridad_asignada = num_prioridad;//para luego responder si ese nombre fue asignado y ver que prioridad gana

	return num_ID;

}

void request_pos_ACK(char * ACK){
	//para esta se planea usar ultimo_nombre_asignado y ultima_prioridad_asignada
}


void confirm_pos(char * paquete, int num_ID, /*IP y puerto, hay que ver como se pasan de parametro(que tipo usar)*/){
	int num_req = rand();
	char * r;
	r = (char *)&num_req;
	paquete[0] = r[3];
	paquete[1] = r[2];
	paquete[2] = r[1];
	paquete[3] = r[0];

	char * x;
	x = (char *)&num_ID;
	paquete[4] = x[1]; //revisar
	paquete[5] = x[0];
	
	int num_tarea = 210;
	char * t;
	t = (char *)&num_tarea;
	paquete[6] = t[0];

	//falta pasar a bytes el IP y el puerto
}

void confirm_pos_ACK(char * paquete, int num_req, int num_ID){
	//De que depende el responder con el ID o con 0 aqui? creo que siempre se responde con el ID
	/*creo que en los metodos ACK el paquete deberia tener el mismo numero de request que el
	 metodo al que le esta respondiendo e.g se manda un confirm_pos con num request 45 entonces se responde con un confirm_pos_ACK con el num_request 45 tambien*/

	char * r;
	r = (char *)&num_req;
	paquete[0] = r[3];
	paquete[1] = r[2];
	paquete[2] = r[1];
	paquete[3] = r[0];

	char * x;
	x = (char *)&num_ID;
	paquete[4] = x[1]; //revisar
	paquete[5] = x[0];
	
	int num_tarea = 211;
	char * t;
	t = (char *)&num_tarea;
	paquete[6] = r[3];
	paquete[7] = r[2];
	paquete[8] = r[1];
	paquete[9] = r[0];

	paquete[10] = t[0];


}





