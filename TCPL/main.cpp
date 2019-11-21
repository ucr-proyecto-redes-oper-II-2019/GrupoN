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
    cout << "Tamaño de la ventana: ";
    int port;
    cin >> port;
    char IP[15];
    IP [0] = '1';
    IP [1] = '9';
    IP [2] = '2';
    IP [3] = '.';
    IP [4] = '1';
    IP [5] = '6';
    IP [6] = '8';
    IP [7] = '.';
    IP [8] = '0';
    IP [9] = '.';
    IP [10] = '1';
    IP [11] = '3';
    TCPLite * tcpl = new TCPLite(t, p);
#pragma omp parallel num_threads(4) shared(tcpl,receptor, emisor,IP,port)
    {
        while (1) {
            int nt = omp_get_thread_num();
            switch (nt) {
                case 0:
                    emisor.lock();
                    tcpl->send_timeout();
                    cout << "SE ENVÍO EN TIMEOUT" << endl;
                    emisor.unlock();
                    break;
                case 1:
                    emisor.lock();
                    char paquete[REQMAXSIZE];
                    if(tcpl->send(IP, port, paquete)){
                        cout << "SE LOGRÓ ENVIAR UN PAQUETE" << endl;
                    } else {
                        cout << "NO SE LOGRÓ ENVIAR UN PAQUETE" << endl;
                    }
                    sleep(1);
                    emisor.unlock();
                    break;
                case 2:
                    receptor.lock();
                    tcpl->receive();
                    receptor.unlock();
                    break;
                case 3:
                    receptor.lock();
                    request b;
                    if(tcpl->getPaqueteRcv(&b)){
                        cout << "SE LOGRÓ SACAR UN PAQUETE" << endl;
                    } else {
                        cout << "NO SE LOGRÓ SACAR UN PAQUETE" << endl;
                    }
                    sleep(1);
                    receptor.unlock();
                    break;
                default:
                    break;
            }
        }
    }
    delete tcpl;
    return 0;
}
