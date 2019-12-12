#ifndef REQUEST_H
#define REQUEST_H

#define REQMAXSIZE 1020
#define PACKMAXSIZE 1015
#define IPMAXSIZE 15

/**
 * @brief The request struct contiene la información necesaria para una solicitud
 * como lo es el IP y puerto al cual se le envía la información, ttl, el paquete
 * a enviar y su tamaño en bytes.
 */
struct request{
    char IP[IPMAXSIZE] = {'\0'};
    unsigned short port = 0;
    int ttl = 0;
    int size = 0;
    char paquete[REQMAXSIZE] = {'\0'};
};

#endif // REQUEST_H
