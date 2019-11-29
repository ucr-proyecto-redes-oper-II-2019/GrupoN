#include "TCPL.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <vector>
#include <omp.h>
#include <bits/stdc++.h>
#include "n_naranja.h"

#define PACKETSIZE 1030
//SOLICITUDES
#define CONNECT 200
#define REQUEST_POS 205
#define DISCONNECT 215
#define REMOVE 220
#define CONFIRM_POS 210

//char paquete[PACKETSIZE]
//generarSolcitud()
//metodos de atender
//TCPLite


int main(){
	TCPLite tcpl(20,8088);
	vector<request> cola_de_Connect;
	vector<request> cola_de_RequestPos;
	vector<request> cola_de_Disconnect;
	vector<request> cola_de_Remove;
	vector<request> cola_de_ConfirmPos;
    N_naranja naranja;
	char paquete[PACKETSIZE];
	int port;
	char * IP;
	int bandera = 0;

	while(1){

		//4 hilos para manejar tcpl y los otros 4 son para cada solicitud
    #pragma omp parallel num_threads(7) shared(paquete,tcpl,port,IP,bandera, cola_de_Connect,cola_de_RequestPos, cola_de_Disconnect,cola_de_Remove,cola_de_ConfirmPos)
		{
			int hilo = omp_get_thread_num();

			if(hilo == 0) {
				#pragma omp critical (receptor)
					{
						tcpl.receive();
					}

			}else if (hilo == 1 ) {
				#pragma omp critical (receptor)
					{
						request req;
						bandera = tcpl.getPaqueteRcv(&req);
                        int * solicitud = reinterpret_cast<int*>(&paquete[6]);

						switch(*solicitud){
							case CONNECT: cola_de_Connect.push_back(paquete);
							break;
							case REQUEST_POS: cola_de_RequestPos.push_back(paquete);
							break;
							case DISCONNECT: cola_de_Disconnect.push_back(paquete);
							break;
							case REMOVE: cola_de_Remove.push_back(paquete);
							break;
							case CONFIRM_POS: cola_de_ConfirmPos.push_back(paquete);
							break;
						}


					}

			}else if(hilo == 2 && bandera){
				#pragma omp critical(emisor)
				{

					for(int i = 0; i < cola_de_Connect.size();++i){
						/*
							revisar si se puede meter a la cola de send (aun hay espacio?) o si se aun no estan instanciados todos los
							nodos del archivo del grafo (aun se pueden unir verdes?)
							para esta ultima revision se tiene que tener un contador cada vez que metamos uno, se actauliza cuando se hace confirm ack
							en caso de no poder hacerlo envia un NACK
							en caso de si poder instanciarlo
							hace un request_pos a todos los vecinos naranjas
							si algun nodo manda un request_pos_ACK negativo se manda un NACK
							si todos le confirmaron, se llama a confirm
							los confirm ACK tambien pueden hacer que se devuelva un NACK
							si todo esto se logro entonces se hace un connect_ACK 

						*/


						vector<char*> ACK;
						int num_ID = cola_de_Connect[i];
						int num_request = rand();

						naranja.connect_ACK(&ACK,cola_de_Connect[i].port,cola_de_Connect[i].IP,num_request);

						for (int i = 0; i < ACK.size(); i++) {
								tcpl.send(cola_de_Connect[i].IP,cola_de_Connect[i].port,ACK[i]);
						}



					}

				}


			}else if(hilo == 3 && bandera){
				#pragma omp critical(emisor)
				{

					for(int i = 0; i < cola_de_RequestPos.size();++i){
						//REquest Pos
						//vector con lista de nodos naranja: publica
						//tcpl revisa bolsa rcv buscando ACK con # request x
						//lo marca en lista ->retorna si logro marcarlo (solo lo logra si es la 1era vez)
						//aumente un contador si lo logra marcar
						//hace confirm cuando el contador == lista.size
						naranja.request_pos(cola_de_RequestPos[i].paquete);
						char paqueteACK[PACKETSIZE];
						naranja.request_pos_ACK(paqueteACK);
						tcpl.send(cola_de_RequestPos[i].IP,cola_de_RequestPos[i].port,paqueteACK);
					}

				}

			}else if(hilo == 4 && bandera){
				#pragma omp critical(emisor)
				{

					for(int i = 0; i < cola_de_Disconnect.size();++i){
						/*
						naranja.disconnect();
						char paqueteACK[PACKETSIZE];
						naranja.disconnect_ACK(paqueteACK);
						tcpl.send(cola_de_Disconnect.IP,cola_de_Disconnect.port,paqueteACK);*/
					}

				}

			}else if(hilo == 4 && bandera){
				#pragma omp critical(emisor)
				{

					for(int i = 0; i < cola_de_Remove.size();++i){
						/*
						naranja.remove();
						char paqueteACK[PACKETSIZE];
						naranja.remove_ACK(paqueteACK);
						tcpl.send(cola_de_Remove.IP,cola_de_Remove.port,paqueteACK);*/
					}

				}

			}else if(hilo == 5 && bandera){
				#pragma omp critical(emisor)
				{

					for(int i = 0; i < cola_de_ConfirmPos.size();++i){
						/*
						char paqueteACK[PACKETSIZE];
						naranja.confirmPos_ACK(paqueteACK);
						tcpl.send(cola_de_ConfirmPos.IP,cola_de_ConfirmPos.port,paqueteACK);*/
					}

				}

			}


			else if(hilo == 6){
				#pragma omp critical(emisor)
				{
					tcpl.send_timeout();
				}

			}
		}

	}

  return 0;
}
