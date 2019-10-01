#ifndef COLA_H
#define COLA_H
#include <iostream>

using namespace std;
typedef char* type_ptr;
typedef char type;

class Cola{
    private:
        int size; // tamano del vector
        int size2;
        type_ptr * vector;
        int nMin; // el paquete más pequeño guardado
        int item_accounter; // cantidad de elementos guardados
        int last_filled; // el paquete más grande guardado

        /*
        Método: copy_to_me
        Parámetros:
            index = la posición en el vector donde se copia
            b = vector fuente
            length = tamano de los datos a copiar
        Ejecución:
                Crea una copia en el punto index del vector con
            tamaño length y empieza a copiar todos los puntos
            del vector fuente
        */
        int copy_to_me(int index, type_ptr b, int length){
            //vector[index]=new type[length];
            for (int i = 0; i < length; i++) {
                vector[index][i]=b[i];
            }
            return 1;
        };
    public:
        /*
        Constructor
        Parámetros:
            size = tamaño del vector
            nMin = inicialización del paquete más pequeño
        Ejecución:
                Inicializa la cola con el tamaño size y lo limpia con ceros
            en cada punto, inicializa el paquete más pequeño con el ingresado
            en nMin y pone la cantidad de elementos en cero.
        */
        Cola(int size, int size2,int nMin){
            last_filled=0;
            this->size = size;
            this->size2 = size2;
            vector = new type_ptr[size];
            for (int i = 0; i < size; i++) {
                vector[i]= new char[size2];
            }

            clean(this->vector,size,size2);

            this->nMin = nMin;
            item_accounter = 0;
        };

        void clean(char ** v,int size,int size2){
            for(int i = 0; i<size; ++i){
                for(int j = 0; j<size2; ++j){
                    v[i][j] = '\0';

                }

            }
        };

        ~Cola(){
        };


        void printVec(char buffer[],int size){
            printf("BUFF: ");
            if(!buffer)
                printf("NONE");
            for(int i = 0; i < size; ++i){
                printf("%c ",buffer[i]);
            }
            printf("\n");
            //fflush(stdout);
        }
        /*
        Método:
            put
        Parámetros:
            num_package = indica el numero de paquete de la información
                          entrante.
            source = puntero a dirección de la información que se quiere
                     ingresar a la cola.
            length = tamaño de la cantidad de elementos apuntados por source
        Ejecución:
                Se fija que el numero de paquete entrante quepa en el rango
            disponible en ese momento en la cola; si cupo, toma el punto
            modular del paquete en el vector y se fija que no exista nada
            en el punto (para no sobreescribir datos ya existentes); si no
            existía nada en el vector llena el ultimo paquete lleno con
            con el paquete actual, copia en el vector de la cola los datos
            señalados por source y aumenta la cantidad de elementos en 1.
        Retornos:
            returna 0 -> el paquete no cupo en el rango del vector.
            retorna !=0 -> se pudo ingresar el paquete o se comprobó su
                           existencia en el vector.
        */
        int put(int num_package,type_ptr source,int length){

          //  printVec(source,length);
            //printf("%d\n",num_package);
            if (nMin <= num_package && ((nMin)+size) > num_package) {
                int pivote = num_package%size;
                //printf("pivote: %d\n",pivote);
                if (esta_vacio(pivote)) {
                  //if(num_package >= last_filled)
                    last_filled=num_package;
                    copy_to_me(pivote, source, length);
                    //printVec(vector[pivote],length);
                    item_accounter++;
                }
                //printf("%d\n",num_package);
                return num_package;
            }
            return 0;
        }
        /*
        Método:
            get_recv
        Parámetros:
            dest = puntero a variable destino donde se guardará lo extraído
                   de la cola.
            length = tamaño de la cantidad de elementos que se pueden copiar
                     en dest.
        Ejecución:
                Calcula la posición modular del paquete más pequeño que se espera
            obtener de la cola, comprueba que el paquete se encuentre en el
            vector, si está, copia la información en el puntero destino, elimina
            los datos recien extraídos de la cola y limpia la posición con un cero,
            aumenta el paquete más pequeño esperado en 1, y decrementa la cantidad
            de elementos en 1.
        Retornos:
            returna 0 -> el paquete no se encontraba en el vector.
            retorna 1 -> se pudo extraer y realizar la operación con éxito.
        */
        int get_recv(type_ptr dest,int length){
            int pivote = nMin%size;
            printf("Pivote get_recv: %d\n",pivote);
            if (!esta_vacio(pivote)) {
                copy_to_others(dest,vector[pivote],length,0,0);
                /*delete vector[pivote];
                vector[pivote]=0;*/
                borrar(pivote);
                //nMin ++;
                item_accounter--;
                return 1;
            }
            return 0;
        };
        /*
        Método:
            get_send
        Parámetros:
            dest = puntero a variable destino donde se guardará lo extraído
                   de la cola.
            num_package = número de paquete a extraer de la cola.
            length = tamaño de la cantidad de elementos que se pueden copiar
                     en dest.
        Ejecución:
                Calcula la posición modular del paquete que se desea extraer, comprueba
            que existe algún dato en esa posición y que el numero de paquete esté
            en el rango de la cola, copia en el puntero destino la información
            de la cola en la posición calculada anteriormente.
                Este método en contraste de get_recv no elimina el elemento extraído
            de la cola.
        Retornos:
            returna 0 -> el paquete no se encontraba en la cola.
            retorna 1 -> se pudo obtener y realizar la operación con éxito.
        */
        int get_send(type_ptr dest, int num_package, int length){
             /*printf("BUFF get send: \n");
                for(int i = 0; i < size; ++i){
                    printf("%c ",dest[i]);
                }
                printf("\n");
                fflush(stdout);*/
            int pivote = num_package%size;
            if (nMin <= num_package && ((nMin+size) > num_package) && !esta_vacio(pivote)) {
                copy_to_others(dest,vector[pivote],length,0,0);
                return 1;
            }
            return 0;
        };
        /*
        Método:
            get_nMin
        Parámetros:
        Ejecución:
                Obtiene el valor del número de paquete más pequeño esperado en
            la cola y lo retorna.
        Retornos:
            returna nMin -> valor del número de paquete más pequeño esperado en
                            la cola.
        */
        int get_nMin(){
            return nMin;
        };
        /*
        Método:
            get_size
        Parámetros:
        Ejecución:
                Obtiene el tamaño de la cola y lo retorna.
        Retornos:
            returna size -> valor del tamaño de la cola.
        */
        int get_size(){
            return size;
        };
        /*
        Método:
            get_nMin
        Parámetros:
        Ejecución:
                Obtiene el valor del número de paquete más grande ingresado en
            la cola y lo retorna.
        Retornos:
            returna last_filled -> valor del número de paquete más grande
                                   ingresado en la cola.
        */
        int get_last(){
            return last_filled;
        };
        /*
        Método:
            set_nMin
        Parámetros:
            num_package = número de paquete que será puesto como el número de
                          más pequeño esperado o ingresado en la por la cola.
        Ejecución:
                Cambia el número de paquete más pequeño esperado por la cola, y
            la recorre limpiando los lugares y la memoria de todos los paquetes
            que se encuentran en la cola que son menores al nuevo número de
            paquete ingresado, luego actualiza el valor del número más pequeño
            ingresado o esperado en la cola con el numero de paquete ingresado,
            e actualiza el valor del ultimo paquete llenado con el de él valor
            del numero de paquete más pequeño ingresado o esperado en la cola
            menos uno.
                Este método solo para cambiar hacia números de paquetes más grandes
            que el número de paquete más pequeño ingreado en la cola.
        Retornos:
            returna n -> valor del número de paquete más grande
                                   ingresado en la cola.
        */
        int set_nMin(int num_package){
            int pivot = 0;
            for (int i = nMin; i < (nMin+size); i++) {
                pivot = i%size;
                if (i < num_package && !esta_vacio(pivot)) {
                    //delete vector[pivot];
                    //vector[pivot] = 0;
                    borrar(pivot);
                    item_accounter--;
                }
            }
            nMin = num_package;
            last_filled = nMin-1;
            return nMin;
        };

        //void set_nMin(int ack){

        //}
        void set_RNmin(int val){
          nMin = val;
        }
        /*
        Método:
            print
        Parámetros:
        Ejecución:
                Imprime todo lo que hay en la cola.
        Retornos:
        */
        int esta_vacio(int pos){
            for(int i = 0; i < size2; i++){
              if(vector[pos][i] != '\0'){
                return 0;
              }
            }
            return 1;
        }

        void borrar(int pos){
          for(int i = 0; i < size2; i++){
            vector[pos][i] = '\0';
          }
        }
        void print(){
            for (int i = 0; i < size; i++) {
                for(int j = 0; j<size2 ; ++j){
                    if (!esta_vacio(i)) {
                        cout << i << " hay datos";
                        break;
                    } else {
                        cout << i << " esta_vacio";
                        break;
                    }
                }
                 cout << endl;
            }
            cout << "RNmin: " << nMin << endl;
            //fflush(stdout);
            return;
        };
        /*
        Método:
            check_status
        Parámetros:
        Ejecución:
                Imprime el valor que hay en cada una de las variables.
        Retornos:
        */
        void check_status(){
            cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << endl;
            printf(" Size:%d\n Item accounter:%d\n nMin:%d\n Last filled:%d\n",
            size, item_accounter, nMin, last_filled);
            cout << "-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-" << endl;
            return;
        }
        /*
        Método:
            copy_to_others
        Parámetros:
            dest = puntero de variable donde se copiarán los datos.
            source = puntero de variable de donde se tomarán los datos a copiar.
            length = tope de cantidad de datos disponibles para copiar.
            dest_index = punto de inicio de la varible destino para copiar los datos.
            source_index = punto de inicio de la varible fuente para copiar los datos.

        Ejecución:
                Recorre desde la posición del indice hasta el tope, copiando el
                valor del punto fuente hacia el de destino.
        Retornos:
        */
        void copy_to_others(type_ptr dest, type_ptr source, int length, int dest_index, int source_index){
            while (dest_index<length && source_index<length) {
                dest[dest_index]=source[source_index];
                dest_index++;
                source_index++;
            }
            return;
        };
};

#endif // COLA_H
