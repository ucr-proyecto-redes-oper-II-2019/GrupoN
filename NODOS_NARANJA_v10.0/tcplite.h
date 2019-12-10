#ifndef TCPLITE_H
#define TCPLITE_H

#include "bolsa.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstdio>
#include <unistd.h>

class TCPLite{
private:
    int sockfd;
    struct sockaddr_in servaddr, cliaddr_send,cliaddr_recv;
    Bolsa * bolsa_send, * bolsa_receive;
    void copy(char*,char*,int);

public:
    TCPLite();
    ~TCPLite();


    /* LLAMADOS CON HILOS EXTERNOS */
    void send_timeout();
    int send(char *IP, unsigned short port, char paquete[], int tam);
    int receive();
    int getPaqueteRcv(request*);

    /* LLAMADOS INTERNOS */
    int send_ACK(char *IP,  unsigned short  port, char paquete[], int tam);
    int getBolsaSize();
    void copyPaq(char * dest, char * vector,int indice,int size);
    void closeSocket();
    void setAll(int,int);

};

#endif // TCPLITE_H
