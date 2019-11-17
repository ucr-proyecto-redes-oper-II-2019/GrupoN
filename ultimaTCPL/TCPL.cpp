#include "TCPL.h"

TCPLite::TCPLite(int tam_bolsas, int puerto_para_recibir){
  bolsa_send = new Bolsa(tam_bolsas);
  bolsa_receive = new Bolsa(tam_bolsas);
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
  servaddr.sin_port = htons(puerto_para_recibir);
}

TCPLite::~TCPLite(){

}
int TCPLite::send_timeout(){
  clock_t inicio;
  double tiempo;
  int numSec;
  while(1){
    inicio = clock();
    while(1){
      tiempo = (double)(clock() - inicio) / CLOCKS_PER_SEC;
      if(tiempo >= 1.5){
        break;
      }
    }
    for(int i = 0; i < bolsa_send->get_size(); i++){
      request req = bolsa_send->get_paquete(i);
      cliaddr_send.sin_port = htons(req.port);
      cliaddr_send.sin_addr.s_addr = inet_addr(req.IP);
      sendto(sockfd,(const char *)req.paquete, REQMAXSIZE, MSG_CONFIRM, (const struct sockaddr *) &cliaddr_send, sizeof(cliaddr_send)); //no se si se manda asi el ack, hay que revisar
      //bolsa_send->get_paquete(i).ttl--;
      bolsa_send->borrar_por_ttl(i);
    }
  }
  return 0;
}

int TCPLite::send_ACK(char IP[15], int port, char paquete[REQMAXSIZE]){
  struct sockaddr_in cliaddr_temp;
  cliaddr_temp.sin_family = AF_INET;
  cliaddr_temp.sin_port = htons(port);
  cliaddr_temp.sin_addr.s_addr = inet_addr(IP);
  sendto(sockfd,(const char *) paquete, REQMAXSIZE, MSG_CONFIRM, (const struct sockaddr *) &cliaddr_temp, sizeof(cliaddr_temp));
  return 0;
}
/*int TCPLite::check_rcvd(){
  for(int i = 0; i < bolsa_receive->get_size();i++){
    request req = get_paquete(i);
    //if(req->paquete, tiene que revisar si el paquete es ack o no
  }
}*/
int TCPLite::send(char IP[15], int port, char paquete[REQMAXSIZE]){
  int sn = random();
  char * p;
	p = (char *)&sn;
	paquete[0] = '\0';
	paquete[1] = p[3];
	paquete[2] = p[2];
	paquete[3] = p[1];
  paquete[4] = p[0];
  return bolsa_send->insertar(IP,port,paquete,1);
}

int TCPLite::receive(){
  socklen_t len;
  char paquete[REQMAXSIZE];
  recvfrom(sockfd, (char *)paquete, REQMAXSIZE, MSG_WAITALL,(struct sockaddr *) &cliaddr_recv, &len);
  request r;
  r.IP = inet_ntoa(cliaddr_recv.sin_addr);
  r.port = ntohs(cliaddr_recv.sin_port);
  for(int i = 0; i < REQMAXSIZE;i++){
      r.paquete[i] = paquete[i];
  }
  int insertado = bolsa_receive->insertar(inet_ntoa(cliaddr_recv.sin_addr),ntohs(cliaddr_recv.sin_port),paquete, 0);
  if(paquete[0] == '\0'){
    for (int i = 0; i < 5; ++i) {
      send_ACK(inet_ntoa(cliaddr_recv.sin_addr),ntohs(cliaddr_recv.sin_port),paquete);
    }

  }else{
      bolsa_send->borrar_confirmado(r);
  }
  return insertado;
}


void TCPLite::copy(char * dest, char * vector,int size){
	for (int i = 0; i < size; ++i){
		dest[i] = vector[i];
	}

}

//Lo saca de la volsa recv y lo borra
int TCPLite::getPaqueteRcv(int i, char * paquete){
  //char paquete[REQMAXSIZE-5];
  for (int i = 0; i < bolsa_receive->get_size(); i++) {
    if(bolsa_receive->get_paquete(i).paquete[0] == '\0' ){
      copy(paquete,&(bolsa_receive->get_paquete(i).paquete[5]),REQMAXSIZE-5);
      bolsa_receive->borrar_recibido(i);
      return 1;
    }
  }
  return 0;//todos son ack o no hay nada en bolsa rcv
}
