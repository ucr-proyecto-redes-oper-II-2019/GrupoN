#include "tcplite.h"
#include <omp.h>
#include "tcplite.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <mutex>
using namespace std;


int main(){
    //mutex receptor;
    //mutex emisor;
    srand(static_cast<unsigned int>(time(nullptr)));
    int t;
    cout << "Tamaño de la ventana: ";
    cin >> t;
    cout << "Puerto de recibo: ";
    int p;
    cin >> p;
    cout << "Puerto de envio: ";
    int port;
    cin >> port;
    char IP[15] = {0};
    IP [0] = '1';
    IP [1] = '0';
    IP [2] = '.';
    IP [3] = '1';
    IP [4] = '.';
    IP [5] = '1';
    IP [6] = '3';
    IP [7] = '8';
    IP [8] = '.';
    IP [9] = '1';
    IP [10] = '7';
    IP [11] = '0';
    //IP [11] = '3';
    int bandera_se_recibio_respuesta = 0;
    TCPLite * tcpl = new TCPLite(t, p);
#pragma omp parallel num_threads(4) shared(tcpl,IP,port,bandera_se_recibio_respuesta)
    {
        while (1) {
            int nt = omp_get_thread_num();
            switch (nt) {
                case 0:
                    //emisor.lock();

                    while(1){
                      #pragma omp critical (emisor)
                      {
                      tcpl->send_timeout();
                      cout << "SE ENVÍO EN TIMEOUT" << endl;
                      }
                      sleep(1);
                    }
                    //emisor.unlock();
                    break;
                case 1:
                    #pragma omp critical (emisor)
                    {
                      //emisor.lock();
                      char paquete[REQMAXSIZE];
		                  int num_req = rand();
		                  char * r;
    	  	            r = reinterpret_cast<char*>(&num_req);
    		              paquete[0] = r[3];
		                  paquete[1] = r[2];
		                  paquete[2] = r[1];
		                  paquete[3] = r[0];
		                  int num_respuesta = 1;
        	            char * x;
        	            x = reinterpret_cast<char*>(&num_respuesta);
        	            paquete[4] = x[1]; //revisar
             	        paquete[5] = x[0];
		                  int num_tarea = 211;
    		              char * t;
    		              t = reinterpret_cast<char*>(&num_tarea);
    		              paquete[6] = t[0];
                      if(tcpl->send(IP, port, paquete)){
                          cout << "SE LOGRÓ METER UN PAQUETE A LA BOLSA_SEND" << endl;
                      } else {
                          cout << "NO SE LOGRÓ METER UN PAQUETE A LA BOLSA_SEND" << endl;
                      }
                      //emisor.unlock();
                    }
            		    sleep(10);
            		    if(bandera_se_recibio_respuesta){
            		    	cout << "se asigno el nombre" ;
            		    }
                    break;
                case 2:
                  #pragma omp critical (receptor)
                    {
                        //dnd sta el suyo
                    //receptor.lock();
                    //cout<<"here\";
                      tcpl->receive();
                    }
                    //receptor.unlock();
                    break;
                case 3:
                    //receptor.lock();
                    #pragma omp critical (receptor)
                    {
                      request b;
                      if(tcpl->getPaqueteRcv(&b)){
                  			 int * solicitud = reinterpret_cast<int *>(&b.paquete[6]);
                  			 if(*solicitud == 201){
                    				char id[2];
                    				id[0] = b.paquete[5];
                    				id[1] = b.paquete[4];
                    				int * num_id = (int*)(&id);
                    				int numID = *num_id;
                    				cout << "mi ID es: "<< numID <<endl;
                    				char id_vecino[2];
                    				id_vecino[0] = b.paquete[16];
                    				id_vecino[1] = b.paquete[15];
                    				int * num_id_vecino= (int*)(&id_vecino);
                    				int numIDVecino = *num_id_vecino;
                    				char ip_vecino[4];
                    				ip_vecino[0] = b.paquete[20];
                    				ip_vecino[1] = b.paquete[19];
                    				ip_vecino[2] = b.paquete[18];
                    				ip_vecino[3] = b.paquete[17];
                    				unsigned int * ip_vecino_num = (unsigned int*)(ip_vecino);
                    				unsigned int x = *ip_vecino_num;
                    				struct sockaddr_in z;
                    				z.sin_addr.s_addr = x;
                    				char * y = inet_ntoa(z.sin_addr);
                    				char puerto_vecino[2];
                    				puerto_vecino[0] = b.paquete[22];
                    				puerto_vecino[1] = b.paquete[23];
                    				unsigned short * puerto_vecino_num = (unsigned short*)(&puerto_vecino);
                    				unsigned short puerto_num = *puerto_vecino_num;
                    			 	cout << "mi vecino es: " << numIDVecino << " IP: " << y << " puerto: " << puerto_num;
                  			 }
                         cout << "SE LOGRÓ SACAR UN PAQUETE" << endl;
                      } else {
                         cout << "NO SE LOGRÓ SACAR UN PAQUETE" << endl;
                      }

                      //receptor.unlock();
                    }
                    sleep(1);
                    break;
                default:
                    break;
            }
        }
    }
    delete tcpl;
    return 0;
}
