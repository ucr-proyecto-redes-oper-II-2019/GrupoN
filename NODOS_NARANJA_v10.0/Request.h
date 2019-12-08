#ifndef REQUEST_H
#define REQUEST_H

#define REQMAXSIZE 1020

struct request{
    char * IP;
    unsigned short port;
    int ttl;
    int size;
    char * paquete;
};

#endif // REQUEST_H
