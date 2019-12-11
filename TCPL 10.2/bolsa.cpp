#include "bolsa.h"

Bolsa::Bolsa(int max){
    this->max = max;
}

void Bolsa::copy(char * dest, char * vector,int size){
    for (int i = 0; i < size; ++i){
        dest[i] = vector[i];
    }
}

bool Bolsa::insertar(char * IP, unsigned short port, char paquete[], int tipo_bolsa, int tam){
    request packet;
    packet.size = tam;
    packet.IP = IP;
    packet.paquete = paquete;
    packet.port = port;

    if(tipo_bolsa == 1){
        packet.ttl = 30;
    }else{
        packet.ttl = 0;
    }
    if(bolsa.size() < max){
        bolsa.push_back(packet);
        return true;
    }
    return false;
}

bool Bolsa::requestIguales(request paqueteACK, request paqueteINFO){
    char numSecACK[5];
    numSecACK[0] = paqueteACK.paquete[1];
    numSecACK[1] = paqueteACK.paquete[2];
    numSecACK[2] = paqueteACK.paquete[3];
    numSecACK[3] = paqueteACK.paquete[4];
    numSecACK[4] = '\0';

    char numSecInfo[5];
    numSecInfo[0] = paqueteINFO.paquete[1];
    numSecInfo[1] = paqueteINFO.paquete[2];
    numSecInfo[2] = paqueteINFO.paquete[3];
    numSecInfo[3] = paqueteINFO.paquete[4];
    numSecInfo[4] = '\0';


    if( strcmp(numSecInfo, numSecACK) == 0 && strcmp(paqueteACK.IP , paqueteINFO.IP) == 0 && paqueteINFO.port == paqueteACK.port){
        return 1;
    }

    return 0;
}

bool Bolsa::borrar_confirmado(request paqueteACK){
    for(int i = 0; i < this->bolsa.size(); ++i){
        if( requestIguales(paqueteACK,bolsa[i]) ){
            bolsa.erase (bolsa.begin()+i);
            return 1;
        }
    }
    return 0;
}

void Bolsa::borrar_por_ttl(int i){
    if(bolsa[i].ttl == 0){
        bolsa.erase(bolsa.begin()+i);
    }
}

void Bolsa::borrar_recibido(int indice){
    bolsa.erase(bolsa.begin()+indice);
}

void Bolsa::decrement_ttl(int indice){
    bolsa[indice].ttl--;
}

int Bolsa::get_size(){
    return bolsa.size();
}

request Bolsa::get_paquete(int indice){
    return bolsa[indice];
}
