#ifndef BOLSA_H
#define BOLSA_H

#include "Request.h"
#include <arpa/inet.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;


class Bolsa{
private:
    int max;
    vector<request> bolsa;
    int requestIguales(request paqueteACK, request paqueteInfo);
    void copy(char*,char*,int);

public:
    Bolsa(int max);
    bool insertar(char * IP, unsigned short port, char paquete[REQMAXSIZE], int tipo_bolsa, int tam);//0 = bolsa recibir , 1 = bolsa envio
    int borrar_confirmado(request paquete);//paquete = paquete con ack , busca en la bolsa para mandar el confirmado
    void borrar_por_ttl(int);
    int get_size();
    request get_paquete(int indice);
    void borrar_recibido(int i);
};

#endif // BOLSA_H
