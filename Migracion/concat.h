#include "stdio.h"
#include <unistd.h>
#include <stdlib.h>


int concatenar(char filename[15], FILE * source,FILE * dest);
int desconcatenar(FILE * dest);


/******************************************************************************/
/**
  @brief Abre el archivo a desempaquetar y lo empieza a desconcatenar

  Está hecho estrictamente para los 3 archivos que genera la migración del
  proceso matrix.c

  @param value state to be written to thew line, mus be 0 or 1

  @returns Retorna el si se logró (1) hacer la operación o no (0)

*/
int desempaquetar(){
    FILE * archivo = fopen("respaldo.dat", "r");
    if (archivo != NULL) {
        desconcatenar(archivo);
        desconcatenar(archivo);
        desconcatenar(archivo);
        fclose(archivo);
    } else {
        return 0;
    }

    return 1;
}

/******************************************************************************/
/**
  @brief Abre el archivo donde serán concatenados los demás archivos, y uno a
  uno empieza a concatenarlos con su información de cabecera

  Está hecho estrictamente para los 3 archivos que genera la migración del
  proceso matrix.c

  @param filename buffer donde biene el nombre de un archivo a empaquetar.
  @param filename1 buffer donde biene el nombre de un archivo a empaquetar
  @param filename2 buffer donde biene el nombre de un archivo a empaquetar.

  @returns Retorna el si se logró (1) hacer todas las operaciones o no (0)

*/
int empaquetar(char filename[15],char filename1[15],char filename2[15]) {
    int hecho = 1;
    FILE * archivo = fopen("respaldo.dat", "w+");

    FILE * fd = fopen(filename, "r");
    if (fd != NULL) {
        concatenar(filename, fd, archivo);
        fclose(fd);
    } else {
        hecho = 0;
    }

    fd = fopen(filename1, "r");
    if (fd != NULL) {
        concatenar(filename1, fd, archivo);
        fclose(fd);
    } else {
        hecho = 0;
    }

    fd = fopen(filename2, "r");
    if (fd != NULL) {
        concatenar(filename2, fd, archivo);
        fclose(fd);
    } else {
        hecho = 0;
    }

    return hecho;
}

/******************************************************************************/
/**
  @brief Toma la información necesaria para la cabecera (cantidad de bytes,
  nombre y extensión del arvhivo), y los guarda con el contenido de cada archivo
  en el archivo destino para concatena.

  @param filename: buffer donde biene el nombre de un archivo a concatenar.
  @param source: descriptor de archivo del archivo fuente.
  @param dest: descriptor de archivo del archivo destino.

  @returns Retorna el si se logró (1) hacer todas las operaciones o no (0)

*/
int concatenar(char filename[15], FILE * source,FILE * dest){
    int tam;

    fprintf(dest, "%s\n", filename);

    fseek(source,0,SEEK_END);
    tam=ftell(source);
    fseek(source,0,SEEK_SET);

    fprintf(dest,"%d\n",tam);

    char dat[4];
    int seguir = 0;
    while (seguir<tam) {
        fread(&dat,sizeof(char),1,source);
        fwrite(&dat,sizeof(char),1,dest);
        usleep(1);
        seguir++;
    }

    fprintf(dest,"\n");

    return 0;
}

/******************************************************************************/
/**
  @brief Toma la información necesaria para la cabecera (cantidad de bytes,
  nombre y extensión del arvhivo), y los guarda con el contenido de cada archivo
  en el archivo destino para concatena.

  @param filename: buffer donde biene el nombre de un archivo a concatenar.
  @param source: descriptor de archivo del archivo fuente.
  @param dest: descriptor de archivo del archivo destino.

  @returns Retorna el si se logró (1) hacer todas las operaciones o no (0)

*/
int desconcatenar(FILE * source){
    int tam;
    char filename[15];
    FILE * dest;
    char dat;

    fscanf(source, "%s\n%d\n", filename,&tam);
    dest = fopen(filename,"w+");
    if (dest == NULL) {
        printf("%s\n", "no se pudo abrir");
        exit(0);
    }
    int seguir = 0;
    fread(&dat,sizeof(char),1,source);

    while (seguir < tam) {
        fwrite(&dat,sizeof(char),1,dest);
        fread(&dat,sizeof(char),1,source);
        usleep(1);
        seguir ++;
    }
    fclose(dest);

    return 0;
}
