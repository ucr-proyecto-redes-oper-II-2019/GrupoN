#include "Bolsa.h"


Bolsa::Bolsa(int max){
	 this->max = max;
}


void Bolsa::copy(char * dest, char * vector,int size){
	for (int i = 0; i < size; ++i){
		dest[i] = vector[i];
	}

}

//0 = bolsa recibir , 1 = bolsa envio
int Bolsa::insertar(char IP[15], int port, char paquete[REQMAXSIZE], int tipo_bolsa){
	struct request packet;
	copy(packet.IP,IP,15);
	copy(packet.paquete, paquete, REQMAXSIZE);
	packet.port = port;

	if(tipo_bolsa == 1){
		packet.ttl = 255;
	}else{
		packet.ttl = 0;
	}

	bolsa.push_back(packet);

	return 1;

}


int Bolsa::requestIguales(request paqueteACK, request paqueteINFO){
	char numSecACK[4];
	numSecACK[0] = paqueteACK.paquete[0];
	numSecACK[1] = paqueteACK.paquete[1];
	numSecACK[2] = paqueteACK.paquete[2];
	numSecACK[3] = paqueteACK.paquete[3];

	char numSecInfo[4];
	numSecInfo[0] = paqueteINFO.paquete[0];
	numSecInfo[1] = paqueteINFO.paquete[1];
	numSecInfo[2] = paqueteINFO.paquete[2];
	numSecInfo[3] = paqueteINFO.paquete[3];


 	if( strcmp(numSecInfo, numSecACK) == 0 && strcmp(paqueteACK.IP , paqueteINFO.IP) == 0 && paqueteINFO.port == paqueteACK.port){
 		return 1;
 	}

 	return 0;
	
}




//paquete = paquete con ack , busca en la bolsa para borrar el confirmado
//para bolsa de recibido
int Bolsa::borrar_confirmado(request paqueteACK){
	for(int i = 0; i < this->bolsa.size(); ++i){
		if( requestIguales(paqueteACK,bolsa[i]) ){
			 bolsa.erase (bolsa.begin()+i);
			 return 1;
		}
		
	}

	return 0;

}

//para bolsa de envio
void Bolsa::borrar_por_ttl(){
	for(int i = 0; i < this->bolsa.size(); ++i){
		if( bolsa[i].ttl == 0 ){
			 bolsa.erase (bolsa.begin()+i);
		}
		
	}

}

int Bolsa::get_size(){
	return bolsa.size();
}

request Bolsa::get_paquete(int indice){
	return bolsa[indice];
}