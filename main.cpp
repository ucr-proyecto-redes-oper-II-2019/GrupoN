#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//ESTA FUNCION RETORNA EL TAMAÑO DE UN ARCHIVO
int main(int argc, char const *argv[]) {
    int h;
    long t;
    FILE * fptr;
    fptr = fopen("Diseño formal","r");
    fseek(fptr, 0L, SEEK_END);
    printf("test.c ocupa %ld bytes", ftell(fptr));
    fclose(fptr);
    return t;
}
