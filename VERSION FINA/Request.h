#ifndef REQUEST_H
#define REQUEST_H

#define REQMAXSIZE 1020

struct request{
    char IP[15];
    unsigned short port = 0;
    int ttl = 0;
    int size = 0;
    char paquete[REQMAXSIZE];
};

#endif // REQUEST_H
