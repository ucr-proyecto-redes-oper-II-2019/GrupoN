/**
* \file concat.h
* \author Guillermo Ramírez Villalta
* \date 31/10/2019
* \brief implementación de la concatenación
*/
#include "stdio.h"
#include <unistd.h>
#include <stdlib.h>
#define BUFFER_SIZE 2*1024*1024
#define NAME_LEN 64


int concatenar(char filename[15], FILE * source,FILE * dest);
int desconcatenar(FILE * dest, int n_arch);
void extraer(FILE * source, FILE * dest, int tam);
void rellenar_comando(char * comando, char * filename){
    char * ch= "chmod +x ";
    for (int i = 0; i < 9; i++) {
        comando[i] = ch[i];
    }
    for (int i = 9; i < 72; i++) {
        comando[i]=filename[i-9];
    }
}


/******************************************************************************/
/**
  @brief Abre el archivo a desempaquetar y lo empieza a desconcatenar.

  @param value state to be written to thew line, mus be 0 or 1

  @returns Retorna el si se logró (1) hacer la operación o no (0)

*/
void desempaquetar(char * filename){
    FILE * archivo = fopen(filename, "rb");
    int num_files=0;
    if (archivo != NULL) {
        fread(&num_files,sizeof(int),1, archivo);
        for (int i = 0; i < num_files; i++) {
            desconcatenar(archivo,i);
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
    FILE * fd_salida;
    FILE * respaldo = fopen(filename[2], "wb+");
    int ca = num_files - 3;

    fwrite(&ca,sizeof(int),1,respaldo);

    for (int i = 3; i < num_files; i++) {
        fd_salida = fopen(filename[i], "rb");
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
  @brief concatenar: Toma la información necesaria para la cabecera (cantidad de bytes,
  nombre y extensión del arvhivo), y los guarda con el contenido de cada archivo
  en el archivo destino para concatena.

  @param filename: buffer donde biene el nombre de un archivo a concatenar.
  @param source: descriptor de archivo del archivo fuente.
  @param dest: descriptor de archivo del archivo destino.

  @returns Retorna el si se logró (1) hacer todas las operaciones o no (0)

*/
int concatenar(char filename[NAME_LEN], FILE * source,FILE * dest){
    int tam;

    fwrite(filename, sizeof(char), NAME_LEN,dest);

    fseek(source,0,SEEK_END);
    tam=ftell(source);
    fseek(source,0,SEEK_SET);

    fwrite(&tam,sizeof(int),1,dest);

    char dat[BUFFER_SIZE];
    extraer(source, dest, tam);
    printf("Archivo: %s\t\t Cantidad de bytes:%d\n", filename, tam);

    return 0;
}

/******************************************************************************/
/**
  @brief desconcatenar: Toma la información necesaria para la cabecera (cantidad de bytes,
  nombre y extensión del arvhivo), y los guarda con el contenido de cada archivo
  en el archivo destino para concatena.

  @param filename: buffer donde biene el nombre de un archivo a concatenar.
  @param source: descriptor de archivo del archivo fuente.
  @param dest: descriptor de archivo del archivo destino.

  @returns Retorna el si se logró (1) hacer todas las operaciones o no (0)

*/
int desconcatenar(FILE * source, int n_arch){
    int tam;
    char filename[NAME_LEN];
    FILE * dest;

    fread(filename,sizeof(char),NAME_LEN,source);
    fread(&tam,sizeof(int),1,source);
    dest = fopen(filename,"wb+");
    if (dest == NULL) {
        printf("%s\n", "no se pudo abrir");
        exit(EXIT_FAILURE);
    }
    extraer(source, dest, tam);
    printf("Archivo: %s\t Cantidad de bytes:%d\n", filename, tam);
    fclose(dest);
    if (n_arch == 0) {
        char comando[72];
        rellenar_comando(comando,filename);
        system(comando);
    }

    return 0;
}

/******************************************************************************/
/**
  @brief extraer: extrae la información de la fuente y en bloques de BUFFER_SIZE
  bytes la va ingresando al destino.

  @param source: descriptor de archivo del fuente.
  @param dest: descriptor de archivo del destino.
  @param tam: tamaño total a extraer del fuente e insertar al destino.

*/
void extraer(FILE * source, FILE * dest, int tam){
    char dat[BUFFER_SIZE];
    int seguir = 0;
    int extraer = 0;
    while (seguir < tam) {
        if ((seguir + BUFFER_SIZE) < tam) {
            extraer = BUFFER_SIZE;
        } else {
            extraer = tam - seguir;
        }
        fread(dat,sizeof(char),extraer,source);
        fwrite(dat,sizeof(char),extraer,dest);
        usleep(1);
        seguir += extraer;
    }
}
