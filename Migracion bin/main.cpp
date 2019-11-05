#include "tcpl.h"
#include <omp.h>
#include <iostream>
#define HILOS 8
using namespace std;

int suma_vectorial(int * v, int tam, int aviso){
    int sum = 0;
    TCPL tc= TCPL(aviso);
    // tomar la decisión dinamicamente la cantidad de los hilos
    int vector[HILOS] = {0,0,0,0,0,0,0,0};
    // verificar la necesidad de la creación de los hilos
    // para ver cosas que son muy pequeñas lo que hay que hacer es ver la medición macro y luego fraccionarla
    #pragma omp parallel for num_threads(HILOS) shared(vector,tc)
    for (int i = 0; i < tam; i++) {
        vector[omp_get_thread_num()] = tc.suma(vector[omp_get_thread_num()], v[i]);
        #pragma omp critical
        tc.aumentar();

    }
    #pragma omp barrier
    for (int i = 0; i < HILOS; i++) {
        sum = tc.suma(sum,vector[i]);
        tc.aumentar();
    }
    cout << "El resultado es : " << sum << endl;
    return sum;
}

void input(int * tam, int * aviso){
    cout << "ingrese el tamaño" << endl;
    cin >> *tam;
    cout << "A los cuantos llamados desea el aviso" << endl;
    cin >> *aviso;
}

void rellenar(int * v, int tam){
    for (int i = 0; i < tam; i++) {
        v[i] = i;
    }
}

void capa_archivo(){
    int aviso;
    int tam;
    input(&tam,&aviso);
    // manejar los posibles errores en tiempo de ejecución
    int vector[tam];
    rellenar(vector, tam);
    suma_vectorial(vector, tam, aviso);
}

int main(int argc, char const *argv[]) {
    capa_archivo();
    return 0;
}
