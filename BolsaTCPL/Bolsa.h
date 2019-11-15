#ifndef BOLSA
#define BOLSA
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
#define REQMAXSIZE 1032

struct request{
  char IP[15];
  int port;
  int ttl; //segundos
  char paquete[REQMAXSIZE];
};

class Bolsa{
private:
    int max;
    vector<request> bolsa;
    int requestIguales(request paqueteACK, request paqueteInfo);
    void copy(char*,char*,int);

  public:
    Bolsa(int max);
    int insertar(char IP[15], int port, char paquete[REQMAXSIZE], int tipo_bolsa);//0 = bolsa recibir , 1 = bolsa envio
    int borrar_confirmado(request paquete);//paquete = paquete con ack , busca en la bolsa para mandar el confirmado
    void borrar_por_ttl(int);
    int get_size();
    request get_paquete(int indice);

};
#endif
