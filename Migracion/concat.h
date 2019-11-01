#include "stdio.h"
#include <unistd.h>
#include <stdlib.h>
#define BUFFER_SIZE 1
#define NAME_LEN 100


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
void desempaquetar(char * filename){
    FILE * archivo = fopen(filename, "r");
    int num_files=0;
    if (archivo != NULL) {
        fscanf(archivo,"%d\n",&num_files);
        for (int i = 0; i < num_files; i++) {
            desconcatenar(archivo);
        }
        fclose(archivo);
    } else {
        fprintf(stderr,"%s\n", "No se pudo abrir el respaldo");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
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
void empaquetar(char ** filename, int num_files) {
    int hecho = 1;
    FILE * fd_salida;
    FILE * respaldo = fopen("respaldo.cat", "w+");
    int ca = num_files - 2;

    fprintf(respaldo, "%d\n", ca);

    for (int i = 2; i < num_files; i++) {
        fd_salida = fopen(filename[i], "r");
        if (fd_salida != NULL) {
            concatenar(filename[i], fd_salida, respaldo);
            fclose(fd_salida);
        } else {
            printf("%s %s\n", "No se pudo concatenar el archivo: ", filename[i]);
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
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

    char dat[BUFFER_SIZE];
    int seguir = 0;
    while (seguir<tam) {
        fread(&dat,sizeof(char),BUFFER_SIZE,source);
        fwrite(&dat,sizeof(char),BUFFER_SIZE,dest);
        usleep(1);
        seguir = seguir + BUFFER_SIZE;
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
    char filename[NAME_LEN];
    FILE * dest;
    char dat[BUFFER_SIZE];

    fscanf(source, "%s\n%d\n", filename,&tam);

    dest = fopen(filename,"w+");
    if (dest == NULL) {
        printf("%s\n", "no se pudo abrir");
        exit(EXIT_FAILURE);
    }
    int seguir = 0;
    while (seguir < tam) {
        fread(dat,sizeof(char),BUFFER_SIZE,source);
        fwrite(dat,sizeof(char),BUFFER_SIZE,dest);
        usleep(1);
        seguir = seguir + BUFFER_SIZE;
    }
    fclose(dest);

    return 0;
}
