#include "n_verde.h"

N_verde::N_verde(char * IP, int puerto){
	this->IP = IP;
	this->puerto = puerto;
	nombre = -1;
}

N_verde::~N_verde(){


}

//4B 2B 1B 2B 2B 2B 2B
void N_verde:: armar_paquete(char * paquete, int num_req, int respuesta,
                            int num_tarea, int num_prioridad, int fuente,
                            int destino, int ttl){
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

    if(fuente!= -1){
        char * q;
        q = reinterpret_cast<char*>(&ttl);
        paquete[9] = q[1];
        paquete[10] = q[0];

        q = reinterpret_cast<char*>(&fuente);
        paquete[11] = q[1];
        paquete[12] = q[0];

        q = reinterpret_cast<char*>(&destino);
        paquete[13] = q[1];
        paquete[14] = q[0];

    }
}


void N_verde::copiar(char * src, char * dest, int tam){
  for(int i = 0 ; i < 15 ; i++){
    dest [i] = '\0';
  }
  for (int i = 0; i < tam; i++) {
    dest[i] = src[i];
  }
}


//COMUNICACION CON NARANJA

void N_verde::connect(char * paquete){
	int num_req = rand();
    int num_tarea = 200;
    char packet[15];
    armar_paquete(packet,num_req,1,num_tarea,0,-1,-1,-1); //revisar
    copiar(packet,paquete,15);

}

void N_verde::disconnect(char * paquete){
	int num_req = rand();
    int num_tarea = 215;
    armar_paquete(paquete,num_req,this->nombre,num_tarea,0,-1,-1,-1); //revisar
}

//COMUNICACION ENTRE VERDES
void N_verde::greet_neighbor(char * paquete){
	int num_req = rand();
    int num_tarea = 100;
    int respuesta = this->nombre; //ID origen
    armar_paquete(paquete,num_req,respuesta,num_tarea,0,-1,-1,-1);

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

	armar_paquete(paquete,num_req,0,101,0,-1,-1,-1);

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

int N_verde::getPuerto(){
    return this->puerto;

}

void N_verde::send_route(char * paquete, Nodos vec){
	int num_req = rand();
    int num_tarea = 118;
    armar_paquete(paquete,num_req,nombre,num_tarea,0,-1,-1,-1); //revisar
	char * temp = reinterpret_cast<char*>(&vec.puerto);
	paquete[16] = temp[1];
	paquete[17] = temp[0];

	temp = reinterpret_cast<char*>(&vec.distancias);
	paquete[18] = temp[3];
	paquete[19] = temp[2];
	paquete[20] = temp[1];
	paquete[21] = temp[0];
}


void N_verde::llenarDatos(char * paquete,int size){

    char ID[4];
    ID[0] = paquete[5];
    ID[1] = paquete[4];
		ID[2] = '\0';
		ID[3] = '\0';
    int * id = reinterpret_cast<int*>(ID);
    this->nombre = *id;

		Nodos nodo;

		char id_vecino[4];
		id_vecino[0] = paquete[16];
		id_vecino[1] = paquete[15];
		id_vecino[2] = '\0';
		id_vecino[3] = '\0';
		int * num_id_vecino= (int*)(&id_vecino);
		int numIDVecino = *num_id_vecino;
		nodo.nombre = numIDVecino;

		if(size > 17){
			char ip_vecino[4];
			ip_vecino[0] = paquete[20];
			ip_vecino[1] = paquete[19];
			ip_vecino[2] = paquete[18];
			ip_vecino[3] = paquete[17];
			int * ip_vecino_num = (int*)(ip_vecino);
			int x = *ip_vecino_num;
			struct sockaddr_in z;
			z.sin_addr.s_addr = x;
			char * IP_vecino = inet_ntoa(z.sin_addr);


			char puerto_vecino[2];
			puerto_vecino[0] = paquete[22];
			puerto_vecino[1] = paquete[21];
			/*puerto_vecino[2] = '\0';
			puerto_vecino[3] = '\0';*/
			short * puerto_vecino_num = (short*)(&puerto_vecino);
			short puerto_num = *puerto_vecino_num;
			nodo.IP = IP_vecino;
			nodo.puerto = puerto_num;
			nodo.instanciado = 1;
			cout<<"mi nombre es:" << this->nombre<< " mi vecino es: " << nodo.nombre << " IP: " << IP_vecino << " puerto: " << puerto_num<<endl;
			vecinos.push_back(nodo);
		}else{
			cout<<"mi nombre es: " << this->nombre << " no tengo vecinos instanciados aha ha pero si tuviera se llamaria: " << nodo.nombre << endl;
		}

		/*if(puerto_num){
			nodo.IP = IP_vecino;
			nodo.puerto = puerto_num;
			nodo.instanciado = 1;
		}*/




}
