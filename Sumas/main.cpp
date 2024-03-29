#include "tcpl.h"
#include <omp.h>
#include <iostream>
using namespace std;
#define CANT_HILOS 12

void limpiar(int* hilos, int tam){
    for (int i = 0; i < tam; i++) {
        hilos[i] = 0;
    }
}

int suma_vectorial(int * v, int tam, int aviso){
    int sum = 0;
    TCPL tc= TCPL(aviso);
    int hilos = omp_get_num_procs();
    hilos = CANT_HILOS*hilos;
    int vector[hilos];
    limpiar(vector, hilos);
    #pragma omp parallel for num_threads(hilos) shared(vector,tc)
    for (int i = 0; i < tam; i++) {
        vector[omp_get_thread_num()] = tc.suma(vector[omp_get_thread_num()], v[i]);
        #pragma omp critical
        tc.aumentar();

    }
    #pragma omp barrier
    for (int i = 0; i < hilos; i++) {
        sum = tc.suma(sum,vector[i]);
        tc.aumentar();
    }
    cout << "El resultado es : " << sum << " , cantidad de hilos "<< hilos << endl;
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
    int vector[tam];
    rellenar(vector, tam);
    suma_vectorial(vector, tam, aviso);
}

int main(int argc, char const *argv[]) {
    capa_archivo();
    return 0;
}
