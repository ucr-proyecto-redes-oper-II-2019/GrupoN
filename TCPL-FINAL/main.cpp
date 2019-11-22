#include "tcplite.h"
#include <omp.h>
#include "tcplite.h"
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <mutex>
using namespace std;


int main(){
    mutex receptor;
    mutex emisor;
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
    IP [9] = '4';
    IP [10] = '2';
    //IP [11] = '3';
    TCPLite * tcpl = new TCPLite(t, p);
#pragma omp parallel num_threads(4) shared(tcpl,receptor, emisor,IP,port)
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
                      if(tcpl->send(IP, port, paquete)){
                          cout << "SE LOGRÓ METER UN PAQUETE A LA BOLSA_SEND" << endl;
                      } else {
                          cout << "NO SE LOGRÓ METER UN PAQUETE A LA BOLSA_SEND" << endl;
                      }
                      //emisor.unlock();
                    }
		            sleep(1);
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
