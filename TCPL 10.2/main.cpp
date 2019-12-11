#include <cstdlib>
#include <iostream>
#include <tcplite.h>
#include <thread>
#include <signal.h>
#include <Request.h>

#define MIN_VALUES 2
using namespace std;

struct memory {
    request reqst_snd;
    request reqst_rcv;
    bool lleno_snd = false;
    bool lleno_rcv = true;
};

static memory* shmptr;
static TCPLite * tcpl;

void enviar(){
    while (1) {
        if(shmptr->lleno_snd){
            tcpl->send(shmptr->reqst_snd.IP, shmptr->reqst_snd.port, shmptr->reqst_snd.paquete, shmptr->reqst_snd.size);
            shmptr->lleno_snd = false;
        }
        usleep(1);
    }
}

void recibir(){
    while (1) {
        if(!shmptr->lleno_rcv){
            tcpl->getPaqueteRcv(&shmptr->reqst_rcv);
            shmptr->lleno_rcv = true;
        }
        usleep(1);
    }
}

int main(int argc, char *argv[]) {

    /*
     * Si la cantidad de argumentos es menor al mínimo
     * salga con una señal de fallo
     */
    if(argc < MIN_VALUES){
        cout << "Argumentos insuficientes" << endl;
        exit(EXIT_FAILURE);
    }

    tcpl = new TCPLite(atoi(argv[1]),atoi(argv[2]));
    shmptr = new memory();

    thread sto_t(&TCPLite::send_timeout, tcpl);
    thread rcvt_t(&TCPLite::receive, tcpl);
    thread snd_t(enviar);
    thread rcv_t(recibir);

    sto_t.join();
    rcvt_t.join();
    rcv_t.join();
    snd_t.join();

    cout << "ENTRA\n";
    delete tcpl;

    return EXIT_SUCCESS;
}
