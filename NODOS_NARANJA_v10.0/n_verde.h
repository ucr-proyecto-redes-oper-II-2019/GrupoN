#ifndef N_VERDE_H
#define N_VERDE_H
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <bits/stdc++.h>
using namespace std;


class N_verde{
private:
    class Nodos{
	    public:
	        char * IP = nullptr;
	        int puerto = -1;
	        int nombre;
	        int instanciado;
	    };

    char * IP;
    int puerto;
    int nombre; //ID
    vector<Nodos> vecinos;
    vector<char*> files; //primeros dos bytes tienen el numero de segmento,

    void armar_paquete(char * paquete, int num_req, int respuesta,
                        int num_tarea, int num_prioridad, int fuente,
                        int destino, int ttl);

public:
	N_verde();
	~N_verde();

	//de verde a naranja:
	void connect(char * paquete);
	void disconnect(char * paquete);

	//de verde a verde:
	void greet_neighbor(char * paquete);
	void greet_neighbor_ACK(char * paquete, int num_req);
	void file_exists(char * paquete, char * nombre);
	void file_exists_ACK(char * paquete, char * nombre);
	void locate_file(char * paquete);
	void locate_file_ACK(char * paquete);
	void remove_file(char * paquete);
	void remove_file_ACK(char * paquete);
	void put_file(char * paquete);
	void put_file_ACK(char * paquete);
	void get_file(char * paquete);
	void get_file_ACK(char * paquete);

	//de verde a azul:
	void file_complete_ACK(char * paquete,int num_req);
	void locate_file_ACK_final(char * paquete,int num_req);
	void remove_file_ACK_final(char * paquete,int num_req);
	void put_file_ACK_final(char * paquete,int num_req);
	void get_file_ACK_final(char * paquete,int num_req);
	void exec_ACK(char * paquete,int num_req);
	void exec_stop_ACK(char * paquete,int num_req);



};
#endif
