#ifndef BOLSA_H
#define BOLSA_H

#include "Request.h"
#include <arpa/inet.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <string.h>
using namespace std;


class Bolsa{
private:
    int max; ///< Tamaño máximo de la ventana.
    vector<request> bolsa; ///< Vector donde se guardan las solicitudes.

    /**
     * @brief requestIguales comprueba si dos request son iguales, examinando
     * sus contenidos.
     * @param paqueteACK paquete a comprobar.
     * @param paqueteInfo paquete a comprobar.
     * @return 1 si son iguales y 0 si no son iguales.
     */
    bool requestIguales(request paqueteACK, request paqueteInfo);

    /**
     * @brief copy copia el contenido del fuente en el destino,
     * requiere que el tamaño del destino sea mayor o igual al
     * rango.
     * @param source buffer de donde se extrae la información a
     * copiar.
     * @param dest buffer donde se inserta la información a
     * copiar.
     * @param range cantidad de bytes que se desean copiar.
     */
    void copy(char * source,char * dest,int range);

    /**
     * @brief clear debe limpiar los datos que hay en el source con
     * '\0'.
     * @param source buffer que se desea limpiar.
     * @param size tamaño del buffer por limpiar.
     */
    void clear(char * source, int size);
public:

    /**
     * @brief Bolsa constructor que inicializa el máximo de tamaño de
     * máximo de la bolsa.
     * @param max tamaño máximo de la bolsa.
     */
    Bolsa(int max);

    /**
     * @brief insertar crea una solicitud y la inserta en la bolsa.
     * @param IP buffer con la dirección IP.
     * @param port puerto a asignar.
     * @param paquete buffer con el paquete a transportar.
     * @param tipo_bolsa describe si es la bolsa de recibir o de enviar.
     * @param tam indica el tamaño del paquete.
     * @return 1 si logró la inserción, y 0 si no.
     */
    bool insertar(char * IP, unsigned short port, char *paquete, int tipo_bolsa, int tam);

    /**
     * @brief borrar_confirmado borra el request de la bolsa.
     * @param paquete request a eliminar.
     * @return 1 si logró borrarlo y 0 si no.
     */
    bool borrar_confirmado(request paquete);

    /**
     * @brief borrar_por_ttl recorre la bolsa disminuyendo los ttl,
     * si el ttl llega a 0, elimina la solicitud.
     */
    void borrar_por_ttl(int);

    /**
     * @brief get_size obtiene el tamaño de la bolsa.
     * @return el tamaño de la bolsa.
     */
    int get_size();

    /**
     * @brief get_paquete obtiene el paquete que se encuentra en la
     * posición índice de la bolsa.
     * @param indice posición a obtener
     * @return request.
     */
    request get_paquete(int indice);

    /**
     * @brief borrar_recibido borra la posición indice de la bolsa.
     * @param indice posición a borrar.
     */
    void borrar_recibido(int indice);

    /**
     * @brief decrement_ttl decrementa el ttl de la solicitud
     * especificada.
     * @param indice posición de la solicitud a decrementar
     */
    void decrement_ttl(int indice);
};

#endif // BOLSA_H
