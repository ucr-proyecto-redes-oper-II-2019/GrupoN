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

class Nodos{
  public:
      char * IP = nullptr;
      int puerto = -1;
      int nombre;
      int instanciado;
        int distancias = -1 ;
  };

class N_verde{
  private:
    char * IP;
    int puerto;
    int nombre; //ID
    vector<char*> files; //primeros dos bytes tienen el numero de segmento,
    vector<Nodos> vecinos;
    void armar_paquete(char * paquete, int num_req, int respuesta,
                        int num_tarea, int num_prioridad, int fuente,
                        int destino, int ttl);
    void copiar(char * src, char * dest, int tam);

public:
	 N_verde(char * IP, int puerto);
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
  void send_route(char * paquete, Nodos vec);

	//de verde a azul:
	void file_complete_ACK(char * paquete,int num_req);
	void locate_file_ACK_final(char * paquete,int num_req);
	void remove_file_ACK_final(char * paquete,int num_req);
	void put_file_ACK_final(char * paquete,int num_req);
	void get_file_ACK_final(char * paquete,int num_req);
	void exec_ACK(char * paquete,int num_req);
	void exec_stop_ACK(char * paquete,int num_req);

	int getPuerto();
	void llenarDatos(char*,int); //despues de hacer connect llena con los datos que le proporciona el naranja
  vector<Nodos> getVecinos();
  void setVecino(int ID,char * IP, int puerto);

};
#endif
