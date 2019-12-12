#include <cstdlib>
#include <iostream>
#include <tcplite.h>
#include <thread>
#include <signal.h>
#include <Request.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>

#define MIN_VALUES 5
using namespace std;

/**
 * @brief The memory struct es el struct para la memoria compartida
 * donde se guarda tanto los request de las solicitudes que se quieren
 * enviar como los request de las solicitudes que fueron recibidas.
 */
struct memory {
    request reqst_snd;
    request reqst_rcv;
    bool lleno_snd = false;
    bool lleno_rcv = true;
};

static memory* shmptr; ///< Puntero hacia la memoria compartida.
static int shmid; ///< Identificador de la memoria compartida.
static TCPLite * tcpl; ///< Puntero hacia la clase tcpl.
static sem_t * sem_send; ///< Puntero al semáforo nombrado de envío, el cual sincroniza con el nodo asociado.
static sem_t * sem_recv; ///< Puntero al semáforo nombrado de recibo, el cual sincroniza con el nodo asociado.
static char * sem_name_1; ///< Nombre del semáforo de envío.
static char * sem_name_2; ///< Nombre del semáforo de recibo.

/**
 * @brief make_shm crea la memoria compartida.
 * @param key clave para la memoria compartida.
 * @param shmid identificador de la memoria comartida.
 * @return puntero a la memoria compartida.
 */
memory * make_shm(int key, int * shmid){
    *shmid = shmget(key, sizeof(memory), IPC_CREAT | 0666);
    if (*shmid < 0){
        perror("make_shm error");
        delete tcpl;
        exit (EXIT_FAILURE);
    }
    return (memory*)shmat(*shmid, nullptr, 0);
}

/**
 * @brief destroy_shm destruye la memoria compartida y quita el enlace.
 */
void destroy_shm(){
    shmdt(shmptr);
    shmctl(shmid,0,nullptr);
}

/**
 * @brief make_sem crea los semáforos nombrados.
 * @param name_s nombre del semáforo de envío.
 * @param name_r nombre del semáforo de recibo.
 */
void make_sem(char * name_s, char * name_r){
    sem_name_1 = name_s;
    sem_name_2 = name_r;
    sem_send = sem_open(sem_name_1,O_RDWR);
    if (sem_send == SEM_FAILED) {
        perror("sem_send failed");
        destroy_shm();
        delete tcpl;
        exit(EXIT_FAILURE);
    }

    sem_recv = sem_open(sem_name_2,O_RDWR);
    if (sem_recv == SEM_FAILED) {
        perror("sem_recv failed");
        destroy_shm();
        delete tcpl;
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief destroy_sem cierra y quita el enlace de los semáforos
 * nombrados.
 */
void destroy_sem(){
    sem_close(sem_send);
    sem_close(sem_recv);
    sem_unlink(sem_name_1);
    sem_unlink(sem_name_2);
}

/**
 * @brief handler captura la señal y si es SIGINT, da un final adecuado al programa.
 * @param signal señal capturada.
 */
void handler(int signal){
    if(signal == SIGINT){
        destroy_shm();
        destroy_sem();
        delete tcpl;
    }
}

/**
 * @brief enviar toma desde la memoria compartida el request y lo envía al TCPLite,
 * para posteriormente ser enviado a su destino. Tiene la función de consumidor de
 * las solicitudes salientes del nodo asociado.
 */
void enviar(){
    while (1) {
        sem_wait(sem_send);
        if(shmptr->lleno_snd){
            tcpl->send(shmptr->reqst_snd.IP, shmptr->reqst_snd.port, shmptr->reqst_snd.paquete, shmptr->reqst_snd.size);
            shmptr->lleno_snd = false;
        }
        sem_post(sem_send);
    }
}

/**
 * @brief recibir toma el request del paquete y posteriormente lo ingresa en la
 * memoria compartida para que el nodo asociado lo tome. Tiene la función de
 * productor de las solicitudes entrantes del nodo asociado.
 */
void recibir(){
    while (1) {
        sem_wait(sem_recv);
        if(!shmptr->lleno_rcv){
            tcpl->getPaqueteRcv(&shmptr->reqst_rcv);
            shmptr->lleno_rcv = true;
        }
        sem_post(sem_recv);
    }
}

int main(int argc, char *argv[]) {

    /*
     * Si la cantidad de argumentos es menor al mínimo
     * salga con una señal de fallo
     */
    if(argc < MIN_VALUES){
        cout << "Argumentos insuficientes" << endl;
        system("cat argumentos.txt");
        exit(EXIT_FAILURE);
    }

    /*
     * Se crean todas las estructuras y las herramientas de sincronización
     */
    tcpl = new TCPLite(atoi(argv[1]),atoi(argv[2]));
    shmptr = make_shm(atoi(argv[3]),&shmid);

    make_sem(argv[4], argv[5]);

    /*
     * Se enlaza la señal del SIGINT para cerrar apropiadamente el programa.
     */
    signal(SIGINT, handler);

    thread sto_t(&TCPLite::send_timeout, tcpl);
    thread rcvt_t(&TCPLite::receive, tcpl);
    thread snd_t(enviar);
    thread rcv_t(recibir);

    sto_t.join();
    rcvt_t.join();
    rcv_t.join();
    snd_t.join();

    delete tcpl;

    return EXIT_SUCCESS;
}
