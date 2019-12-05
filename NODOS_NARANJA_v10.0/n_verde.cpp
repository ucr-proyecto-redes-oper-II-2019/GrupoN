#include "n_verde.h"

N_verde::N_verde(){
	IP = nullptr;
	puerto = -1;
	nombre = -1;
}

N_verde::~N_verde(){


}

//4B 2B 1B 2B 2B 2B 2B
void N_verde:: armar_paquete(char * paquete, int num_req, int respuesta, int num_tarea, int num_prioridad){
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];

    char * x;
    x = reinterpret_cast<char*>(&respuesta);
    paquete[4] = x[1]; 
    paquete[5] = x[0];
           

    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    paquete[6] = t[0];

    char * p;
    p = reinterpret_cast<char*>(&num_prioridad);
    paquete[7] = p[1];
    paquete[8] = p[0];

}


//COMUNICACION CON NARANJA

void N_verde::connect(char * paquete){
	int num_req = rand();
    int num_tarea = 200;
    armar_paquete(paquete,num_req,1,num_tarea,0);

}

void N_verde::disconnect(char * paquete){
	int num_req = rand();
    int num_tarea = 215;
    armar_paquete(paquete,num_req,this->nombre,num_tarea,0);	
}

//COMUNICACION ENTRE VERDES
void N_verde::greet_neighbor(char * paquete){
	int num_req = rand();
    int num_tarea = 100;
    int respuesta = this->nombre; //ID origen
    armar_paquete(paquete,num_req,respuesta,num_tarea,0);

}

void N_verde::greet_neighbor_ACK(char * paquete, int num_req){
	char ID[2];
	ID[0] = paquete[1];
	ID[1] = paquete[0];
	int * nombre = reinterpret_cast<int*>(ID);

	for (int i = 0; i < vecinos.size(); ++i){
		if(vecinos[i].nombre == *nombre){
			vecinos[i].instanciado = 1;
			break;
		}
	}

	armar_paquete(paquete,num_req,0,101,0);

} 


void N_verde::file_exists(char * paquete, char * nombre){
	int num_req = rand();
    int num_tarea = 102;
    int respuesta = -1; //Pregunta por cualquier segmento 

   // armar_paquete(paquete,num_req,respuesta,num_tarea,/*tamano del nombre??*/);	


}


void N_verde::file_exists_ACK(char * paquete, char * nombre){
	int num_req = rand();
    int num_tarea = 102;
    int respuesta = 0;

    for (int i = 0; i < files.size(); ++i){
    	//if(){

    	//}
    }


}

void N_verde::locate_file(char * paquete){

}

void N_verde::locate_file_ACK(char * paquete){

}

void N_verde::remove_file(char * paquete){

}

void N_verde::remove_file_ACK(char * paquete){

}

void N_verde::put_file(char * paquete){

}

void N_verde::put_file_ACK(char * paquete){

}

void N_verde::get_file(char * paquete){

}

void N_verde::get_file_ACK(char * paquete){

}

//COMUNICACION CON AZUL

void N_verde::file_complete_ACK(char * paquete,int num_req){

}

void N_verde::locate_file_ACK_final(char * paquete,int num_req){

}

void N_verde::remove_file_ACK_final(char * paquete,int num_req){

}

void N_verde::put_file_ACK_final(char * paquete,int num_req){

}

void N_verde::get_file_ACK_final(char * paquete,int num_req){

}

void N_verde::exec_ACK(char * paquete,int num_req){

}

void N_verde::exec_stop_ACK(char * paquete,int num_req){

}


