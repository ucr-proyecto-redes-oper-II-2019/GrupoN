#ifndef N_NARANJA_H
#define N_NARANJA_H
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <arpa/inet.h>
#include <string.h>
#include <bits/stdc++.h>
using namespace std;
//#define tam_paq 9

class Nodos{
public:
    char * IP = nullptr;
    int puerto = -1;
    int nombre;
};

class N_verde{
  public:
    char * IP;
    int puerto;
    int nombre;
    vector<Nodos> vecinos;
};

class N_naranja{
private:
    int ID;//se lee del archivo cuando encuentre su IP
    char * IP; //configurar en constructor
    int puerto;  //configurar en constructor
    vector<int> vector_nombres_usados;
    vector<N_verde> grafo_verdes;
    vector<Nodos> naranjas;
    int esperando_request_pos_ACK;
    int ultimo_nombre_asignado;
    int ultima_prioridad_asignada;
    int revisar_ID(int ID);
    int borrar_ID(int ID);
    void llenar_grafo_verdes(string archivo_grafos_verdes); // sacar los datos del archivo *metodo que falta*
    int configurar(string archivo_configuracion);//sacar datos del archivo de configuracion para saber de los vecinos naranja y el ID asignado a este nodo
    int encontrar_nombre(char * IP,int puerto, vector<Nodos> * vecinos);// busqueda lineal del nombre de nodo verde *metodo que falta*
    //agregar ttl a todos los paquetes
public:
    N_naranja(string archivo_grafo_verdes, string archivo_configuracion,char * IP,int);
    ~N_naranja();
    int request_pos(char * paquete, int num_req, int num_ID);
    void request_pos_ACK(char * ACK,int num_req,int num_ID, int num_prioridad);
    void confirm_pos(char * paquete, int num_ID, int num_req, char * IP, int port/*IP y puerto, hay que ver como se pasan de parametro(que tipo usar)*/);
    void confirm_pos_ACK(char * ACK, int num_req, int num_ID);
    void disconnect_ACK(char * ACK, int num_req, int num_ID);
    void remove(char * paquete, int num_ID);
    void remove_ACK(char * ACK, int num_ID, int num_req);
    void connect_ACK(vector<char*> * ACK, int puerto,char * IP, int num_request);
    void fill_header(char * paquete, int num_request, int i_c_r, int tarea_realizar);
    void fill_neighbour(char * paquete, int id, unsigned int ip, unsigned short puerto, int size);
    int getID();
    int getPuerto();
    vector<Nodos> getNaranjas();

};

#endif // N_NARANJA_H
