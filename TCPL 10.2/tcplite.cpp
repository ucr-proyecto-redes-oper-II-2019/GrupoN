#include "tcplite.h"

TCPLite::TCPLite(int bolsas_len, int port){
    bolsa_send = new Bolsa(bolsas_len);
    bolsa_receive = new Bolsa(bolsas_len);
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr_send,0 , sizeof(cliaddr_send));
    memset(&cliaddr_recv,0 , sizeof(cliaddr_recv));

    cliaddr_send.sin_family = AF_INET;
    cliaddr_recv.sin_family = AF_INET;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(static_cast<unsigned short>(port));
    if ( bind(sockfd, reinterpret_cast<const struct sockaddr *>(&servaddr), sizeof(servaddr)) < 0 ) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

TCPLite::~TCPLite(){
    closeSocket();
}

/***************************** LLAMADOS EXTERNOS *****************************/

void TCPLite::send_timeout(){
    while (1) {
        critical.lock();

        clock_t inicio;
        double tiempo;
        inicio = clock();
        while(1){
            tiempo = (double)(clock() - inicio) / CLOCKS_PER_SEC;
            if(tiempo >= 3){
                break;
            }
        }
        for(int i = 0; i < bolsa_send->get_size(); i++){
            request req = bolsa_send->get_paquete(i);
            cliaddr_send.sin_port = htons(req.port);
            cliaddr_send.sin_addr.s_addr = inet_addr(req.IP);
            sendto(sockfd,(const char*)(req.paquete),(unsigned long)(req.size),
                   MSG_CONFIRM, (const struct sockaddr *)(&cliaddr_send), sizeof(cliaddr_send));
            bolsa_send->borrar_por_ttl(i);
        }

        critical.unlock();
        usleep(1);
    }
}

bool TCPLite::send(char * IP, unsigned short port, char paquete[], int tam){
    critical.lock();

    char temp[tam];
    copy(temp,paquete,tam);
    char pack[HEADERSIZE + tam];
    int sn = static_cast<int>(random());
    char * p;
    p = reinterpret_cast<char *>(&sn);
    pack[0] = '\0';
    pack[1] = p[3];
    pack[2] = p[2];
    pack[3] = p[1];
    pack[4] = p[0];
    for(int i=0; i<tam; i++){
        pack[i+HEADERSIZE]=temp[i];
    }

    bool x = bolsa_send->insertar(IP,port,pack,1, tam+HEADERSIZE);

    critical.unlock();
    return x;
}

void TCPLite::receive(){
    while (1) {
        critical.lock();

        socklen_t len;
        char paquete[REQMAXSIZE];
        cout<<"Entra a recv antes de recvfrom()\n";
        int bytes_recv = recvfrom(sockfd,static_cast<char *>(paquete), REQMAXSIZE, MSG_WAITALL,
                                  reinterpret_cast<struct sockaddr *>(&cliaddr_recv), &len);
        cout << "SE RECIBIO ALGO\n";
        request r;
        r.IP = inet_ntoa(cliaddr_recv.sin_addr);
        r.port = ntohs(cliaddr_recv.sin_port);
        r.paquete = new char[bytes_recv];
        for(int i = 0; i < bytes_recv;i++){
            r.paquete[i] = paquete[i];
        }
        int insertado = -1;
        if(paquete[0] == '\0'){
            insertado = bolsa_receive->insertar(r.IP,r.port,r.paquete, 0, bytes_recv);
            for (int i = 0; i < 5; ++i) {
                cout<<"mandando ACK\n";
                char ack[5];
                ack[0] = static_cast<char>(1);
                ack[1] = paquete[1];
                ack[2] = paquete[2];
                ack[3] = paquete[3];
                ack[4] = paquete[4];
                send_ACK(r.IP,r.port,ack,bytes_recv);
                usleep(1);
            }
        }else{
            bolsa_send->borrar_confirmado(r);
        }

        critical.unlock();
        usleep(1);
    }
}

int TCPLite::getPaqueteRcv(request * req) {
    critical.lock();
    cout<<"bolsa_receive size: "<<bolsa_receive->get_size()<<endl;
    for (int i = 0; i < bolsa_receive->get_size(); i++) {
        if(bolsa_receive->get_paquete(i).paquete[0] == '\0' ){
            char pack[bolsa_receive->get_paquete(i).size-HEADERSIZE];
            req->paquete = pack;
            copyPaq(req->paquete,bolsa_receive->get_paquete(i).paquete,HEADERSIZE, bolsa_receive->get_paquete(i).size-HEADERSIZE);
            req->port = bolsa_receive->get_paquete(i).port;
            req->IP = bolsa_receive->get_paquete(i).IP;
            req->size = bolsa_receive->get_paquete(i).size-HEADERSIZE;
            bolsa_receive->borrar_recibido(i);

            critical.unlock();
            return 1;
        }
    }

    critical.unlock();
    return 0;
}

/***************************** LLAMADOS INTERNOS *****************************/

int TCPLite::send_ACK(char * IP, unsigned short port, char paquete[], int tam){
    struct sockaddr_in cliaddr_temp;
    cliaddr_temp.sin_family = AF_INET;
    cliaddr_temp.sin_port = htons(port);
    cliaddr_temp.sin_addr.s_addr = inet_addr(IP);
    sendto(sockfd,(const char *)(paquete), tam, MSG_CONFIRM, (const struct sockaddr *)(&cliaddr_temp), sizeof(cliaddr_temp));
    return 0;
}

void TCPLite::copy(char * dest, char * source, int size){
    for (int i = 0; i < size; ++i){
        dest[i] = source[i];
    }
}

void TCPLite::copyPaq(char * dest, char * vector,int indice,int size){
    for (int i = indice; i < size; ++i){
        dest[i] = vector[i];
    }
}

int TCPLite::getBolsaSize(){
    return bolsa_receive->get_size();
}

void TCPLite::closeSocket(){
    delete bolsa_send;
    delete bolsa_receive;
    close(sockfd);
    cout<<"closed socket\n";
}

