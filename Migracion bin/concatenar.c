/**
* \file concatenar.c
* \author Guillermo Ramírez Villalta
* \date 31/10/2019
* \brief implementación de la concatenación
*/
#include<stdio.h>
#include <curses.h>
#include <string.h>
#include "concat.h"
#define  CONCA "-c"
#define  DESCONCA "-d"
#define  HELP "-h"

/******************************************************************************/
/**
  @brief print_help: Despliega un cuadro de ayuda.
*/
void print_help(){
    system("cat ayuda.txt");
    exit(EXIT_SUCCESS);
}

/******************************************************************************/
/**
  @brief Toma las opciones dadas en argv las valída y selecciona una
  acción por hacer.

  @param argc: cantidad de argumentos.
  @param argv: conjunto de argumentos.

  @returns Retorna 0 para concatenar, 1 para desconcatenar, o despliega la ayuda.

*/
int seleccionar(int argc, char ** argv){
    if (argc>1) {
        if (strcmp(argv[1],CONCA)==0) {
            if (argc >= 3) {
                return 0;
            }
        }
        if (strcmp(argv[1],DESCONCA)==0) {
            if (argc == 3) {
                return 1;
            }
        }
        if (strcmp(argv[1],HELP)==0) {
            if (argc == 2) {
                print_help();
            }
        }
    }
    printf("%s\n", "ERROR DE INGRESO = ingrese \"-h\" para ayuda.");
    exit(EXIT_FAILURE);
}

/******************************************************************************/
/**
  @brief Llamados principales del programa.

  @param argc: cantidad de argumentos.
  @param argv: conjunto de argumentos.

  @returns Retorna 0.

*/
int main(int argc, char const *argv[]) {
    char ** arch = (char**)argv;
    int select = seleccionar(argc,arch);
    switch (select) {
        case 0:
            empaquetar(arch, argc);
            break;
        case 1:
            desempaquetar(arch[2]);
            break;
    }
    return 0;
}
