#include "tcplite.h"

TCPLite::TCPLite(int tam_bolsas, int puerto_para_recibir){
  bolsa_send = new Bolsa(tam_bolsas);
  bolsa_receive = new Bolsa(tam_bolsas);
  if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }
  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr,0 , sizeof(cliaddr));
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
  while(1){
    inicio = clock();
    while(1){
      time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
      if(time_spent >= 1.5){
        break;
      }
    }
    for(int i = 0; i < bolsa_send.get_size(); i++){
      request req = bolsa_send.get_paquete(i);
      cliaddr_send.sin_port = htons(req->port);
      cliaddr_send.sin_addr.s_addr = inet_addr(req->IP);
      sendto(sockfd,(const char *)req->paquete, REQMAXSIZE, MSG_CONFIRM, (const struct sockaddr *) &cliaddr_send, sizeof(cliaddr_send)); //no se si se manda asi el ack, hay que revisar
      --bolsa_send.get_paquete(i).ttl;
      bola_send.borrar_por_ttl(i);
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
  return bolsa_send->insertar(IP,port,paquete,1);
}

int TCPLite::receive(){
  socklen_t len;
  char paquete[REQMAXSIZE];
  recvfrom(sockfd, (char *)paquete, PACKAGE_SIZE, MSG_WAITALL,(struct sockaddr *) &cliaddr_recv, &len);
  request r;
  r.IP = inet_ntoa(cliaddr_recv.sin_addr);
  r.port = ntohs(cliaddr_recv.sin_port);
  for(int i = 0; i < REQMAXSIZE;i++){
      r.paquete[i] = paquete[i];
  }
  int insertado = bolsa_receive.insertar(inet_ntoa(cliaddr_recv.sin_addr),ntohs(cliaddr_recv.sin_port),paquete, 0);
  if(paquete[0] == '\0'){
      send_ACK(inet_ntoa(cliaddr_recv.sin_addr),ntohs(cliaddr_recv.sin_port),paquete);
  }else{
      bolsa_receive->borrar_confirmado(r);
  }
  return insertado;
}
