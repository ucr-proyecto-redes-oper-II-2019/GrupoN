#ifndef TCPLITE_H
#define TCPLITE_H
#define HEADERSIZE 5

#include "bolsa.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <unistd.h>
#include <mutex>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#define SEM_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define INITIAL_VALUE 1

class TCPLite{
private:
    int sockfd;
    //mutex critical;
    struct sockaddr_in servaddr, cliaddr_send,cliaddr_recv;
    Bolsa * bolsa_send, * bolsa_receive;
    sem_t * mutex_bolsa;
    char SEM_NAME[8];

    /**
     * @brief copy copia el contenido del fuente en el destino,
     * requiere que el tamaño del destino sea mayor o igual al
     * rango.
     * @param source buffer de donde se extrae la información a
     * copiar.
     * @param dest buffsem_t * mutex_env;
sem_t * mutex_recv;er donde se inserta la información a
     * copiar.
     * @param range cantidad de bytes que se desean copiar.
     */
    void copy(char * dest,char * source,int range);

public:

    /**
     * @brief TCPLite constructor donde se define el tamaño de las bolsas
     * y el puerto por el cuál se va a recibir la información.
     * @param bolsas_len tamaño de las bolsas.
     * @param port puerto por el cual se va a recibir la
     * información
     */
    TCPLite();
    /**
     * @brief ~TCPLite destructor de la clase, cierra los sockets y
     * elimina la memoria creada.
     */
    ~TCPLite();

    /* LLAMADOS CON HILOS EXTERNOS */

    /**
     * @brief send_timeout recorre la bolsa de envío y envía cada uno
     * de los request que aún contengan ttl positivo y lo decrementa.
     */
    void send_timeout();
    /**
     * @brief send inserta la los datos recibidos como una solicitud
     * y la inserta en la bolsa de envío.
     * @param IP dirección ip a la cual enviar.
     * @param port puerto al cual enviar.
     * @param paquete paquete a ser enviado por TCPLite
     * @param tam tamaño en bytes del paquete.
     * @return true si logró insertarlo y false si no lo logró.
     */
    bool send(char *IP, unsigned short port, char paquete[], int len);
    /**
     * @brief receive recive un paquete desde la red inserta en la bolsa de recibidos.
     * @return
     */
    void receive();
    /**
     * @brief getPaqueteRcv saca de la bolsa de recibidos y lo asigna a la solicitud.
     * @param req solicitud de retorno.
     * @return la cantidad de bytes recibidos.
     */
    int getPaqueteRcv(request * req);

    /* LLAMADOS INTERNOS */

    /**
     * @brief send_ACK envía ack con la información ingresada.
     * @param IP dirección ip a la cual enviar el ack.
     * @param port puerto al cual enviar el ack.
     * @param paquete ack
     * @param tam tamaño del ack.
     * @return
     */
    int send_ACK(char *IP,  unsigned short  port, char paquete[], int tam);
    /**
     * @brief getBolsaSize obtiene el tamaño de las bolsas de envío.
     * @return el tamaño de las bolsas.
     */
    int getBolsaSize();
    /**
     * @brief copyPaq copia solo los datos del paquete sin el header
     * de TCPLite.
     * @param dest paquete destino.
     * @param vector paquete fuente.
     * @param indice punto desde donde iniciar la copia.
     * @param size tamaño del paquete a copiar.
     */
    void copyPaq(char * dest, char * source,int indice,int size);
    /**
     * @brief closeSocket cierra los sockets.
     */
    void closeSocket();
    void setAll(int,int);
    void randstring(char randomString[],int length);
    void borrar_indice_recv(int i);
};

#endif // TCPLITE_H
