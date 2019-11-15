#ifndef TCPLITE_H
#define TCPLITE_H
#include "Bolsa.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class TCPLite{
private:
  int sockfd;
  struct sockaddr_in servaddr, cliaddr_send,cliaddr_recv;
  Bolsa * bolsa_send, * bolsa_receive;
public:
    TCPLite(int tam_bolsas,int puerto_para_recibir);
    ~TCPLite();
    int send_timeout();
    int send_ACK(char IP[15], int port, char paquete[REQMAXSIZE]);
    int check_rcvd();
    int send(char IP[15], int port, char paquete[REQMAXSIZE]);
    int receive();
};

#endif // TCPLITE_H
