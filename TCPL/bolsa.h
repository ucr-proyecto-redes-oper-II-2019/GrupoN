#ifndef BOLSA
#define BOLSA
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;
#define REQMAXSIZE 1032

/**
 * @brief The request struct esta estructura se usa para almacenar cada una de las solicitudes que
 * llegan al sistema.
 *
 * IP: es la dirección de la IP de la fuente o el destino de la solicitud, dependiendo si es una
 * solicitud recibida o enviada.
 *
 * port: número de puerto del cual viene o se dirige la solicitud.
 *
 * ttl: cantidad de segundos que la solicitud se encontrará en espera.
 *
 * paquete: paquete que se desea transmitir.
 */
struct request{
  char  * IP;
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
    void borrar_recibido(int i);

};
#endif
