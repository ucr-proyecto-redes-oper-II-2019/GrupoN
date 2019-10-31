#include<stdio.h>
#include <curses.h>
#include "concat.h"

int main(int argc, char const *argv[]) {

    if (argc>1) {
        if (argc > 2) {
            empaquetar(argv[1],argv[2],argv[3]);
        } else {
            desempaquetar();
            system("chmod +x p");
        }
    } else {
        fprintf(stderr,"%s\n", "Insuficiente cantidad de parámetros");
        exit(EXIT_FAILURE);
    }
    return 0;
}
