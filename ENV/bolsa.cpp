#include "bolsa.h"

Bolsa::Bolsa(int max){
     this->max = max;
 }


void Bolsa::copy(char * dest, char * vector,int size){
    for (int i = 0; i < size; ++i){
        dest[i] = vector[i];
    }

}

//0 = bolsa recibir , 1 = bolsa envio
int Bolsa::insertar(char * IP, int port, char paquete[REQMAXSIZE], int tipo_bolsa){
    request packet;
    //packet.IP = new char[15];
    packet.IP = IP;
    copy(packet.paquete, paquete, REQMAXSIZE);
    packet.port = port;

    if(tipo_bolsa == 1){
        packet.ttl = 30;
    }else{
        packet.ttl = 0;
    }
    if(static_cast<int>(bolsa.size()) < max){
        bolsa.push_back(packet);
        return 1;
    }
    return 0;
}


int Bolsa::requestIguales(request paqueteACK, request paqueteINFO){
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
    cout << "ACK" << endl;
    if( strcmp(numSecInfo, numSecACK) == 0 && strcmp(paqueteACK.IP , paqueteINFO.IP) == 0 && paqueteINFO.port == paqueteACK.port){
        return 1;
    }

    return 0;

}




//paquete = paquete con ack , busca en la bolsa para borrar el confirmado
//para bolsa de recibido
int Bolsa::borrar_confirmado(request paqueteACK){
    for(int i = 0; i < static_cast<int>(this->bolsa.size()); ++i){
        if( requestIguales(paqueteACK,bolsa[static_cast<unsigned long>(i)]) ){
             bolsa.erase (bolsa.begin()+i);
             return 1;
        }

    }

    return 0;

}

//para bolsa de envio
void Bolsa::borrar_por_ttl(int i){
    bolsa[static_cast<unsigned long>(i)].ttl--;
    if(bolsa[static_cast<unsigned long>(i)].ttl == 0){
        bolsa.erase(bolsa.begin()+i);
    }


}

void Bolsa::borrar_recibido(int i){
    bolsa.erase(bolsa.begin()+i);

}

int Bolsa::get_size(){
    return static_cast<int>(bolsa.size());
}

request Bolsa::get_paquete(int indice){
    return bolsa.at(indice);
}
