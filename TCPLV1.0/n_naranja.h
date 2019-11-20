#ifndef N_NARANJA_H
#define N_NARANJA_H
#include <vector>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
#define tam_paq 9


class N_naranja{
private:
    vector<int> vector_nombres_usados;
    int esperando_request_pos_ACK;
    int ultimo_nombre_asignado;
    int ultima_prioridad_asignada;
    int revisar_ID(int ID);
    int borrar_ID(int ID);
public:
    N_naranja();
    int request_pos(char * paquete);
    void request_pos_ACK(char * ACK);
    void confirm_pos(char * paquete, int num_ID /*IP y puerto, hay que ver como se pasan de parametro(que tipo usar)*/);
    void confirm_pos_ACK(char * ACK, int num_req, int num_ID);
    void diconnect_ACK(char * ACK);
    void remove(char * paquete);
    void remove_ACK(char * paquete);

};

#endif // N_NARANJA_H
