#include "n_naranja.h"

N_naranja::N_naranja(){
    srand(static_cast<unsigned int>(time(nullptr)));
    esperando_request_pos_ACK = 0;
}

int N_naranja::revisar_ID(int ID)
{
    for(int i = 0; i < vector_nombres_usados.size(); i++){
        if(vector_nombres_usados.at(i) == ID){
            return 1;
        }
    }
    return 0;
}
int N_naranja::borrar_ID(int ID){
  for(int i = 0; i < vector_nombres_usados.size(); i++){
      if(vector_nombres_usados.at(i) == ID){
            vector_nombres_usados.erase(vector_nombres_usados.begin() + i);
      }
  }
  return 0;
}
int N_naranja::request_pos(char *paquete){
    //se elige el numero de request
    int num_req = rand();
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];

    //se elige el nombre aleatorio ID
    int num_ID = 0;
    while(1){
        num_ID= rand() + 65536 % 65535;
        if(!revisar_ID(num_ID)){
            char * x;
            x = reinterpret_cast<char*>(&num_ID);
            paquete[4] = x[1]; //revisar
            paquete[5] = x[0];
            ultimo_nombre_asignado = num_ID; //para luego responder si ese nombre fue asignado por este nodo
            break;
        }
    }
    //se elige tarea a realizar
    int num_tarea = 205;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    paquete[6] = t[0];

    //se elige la prioridad
    int num_prioridad = rand() + 65536 % 65535;
    char * p;
    p = reinterpret_cast<char*>(&num_prioridad);
    paquete[7] = p[1];
    paquete[8] = p[0];
    ultima_prioridad_asignada = num_prioridad;//para luego responder si ese nombre fue asignado y ver que prioridad gana
    esperando_request_pos_ACK = 1;
    return num_ID;
}
/*los parametros son el paquete que se debe llenar,el numero de request del request_pos originalmente recibido,
el ID del request_pos originalmente recibido, la prioridad del request_pos originalmente recibido,*/
void N_naranja::request_pos_ACK(char *ACK, int num_req,int num_ID, int num_prioridad){
    //para esta se planea usar ultimo_nombre_asignado y ultima_prioridad_asignada
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    ACK[0] = r[3];
    ACK[1] = r[2];
    ACK[2] = r[1];
    ACK[3] = r[0];
    if(esperando_request_pos_ACK && num_ID == ultimo_nombre_asignado){
      //if(esperando_request_pos_ACK){ //si es un nodo que mando un request_pos a los otros nodos recientemente
        if(num_prioridad > ultima_prioridad_asignada){
            int num_respuesta = 1;
            char * x;
            x = reinterpret_cast<char*>(&num_respuesta);
            ACK[4] = x[1];
            ACK[5] = x[0];
            //si la prioridad de la otra persona gano entonces ya no se deberian esperar request_pos_ACK porque se tiene que asignar el ID de nuevo
            esperando_request_pos_ACK = 0;
        }else if(num_prioridad < ultima_prioridad_asignada){
            int num_respuesta = 0;
            char * x;
            x = reinterpret_cast<char*>(&num_respuesta);
            ACK[4] = x[1];
            ACK[5] = x[0];
        }else{//si son iguales
            //se compara el numero de nodo, hay que ver donde se asigna el numero de nodo
        }
    }else{//si el ID no es igual o no se esta esperando ningun request_pos_ACK
        int num_respuesta = 1;
        char * x;
        x = reinterpret_cast<char*>(&num_respuesta);
        ACK[4] = x[1]; //revisar
        ACK[5] = x[0];
    }
    int num_tarea = 206;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    ACK[6] = t[0];
    //el cuerpo va vacio entonces hay que ver que poner para identificarlo como vacio
}

void N_naranja::confirm_pos(char *paquete, int num_ID){
    esperando_request_pos_ACK = 0; //si se esta mandando un confirm_pos significa que ya no se esta esperando ningun request_pos_ACK
    int num_req = rand();
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];

    char * x;
    x = reinterpret_cast<char*>(&num_ID);
    paquete[4] = x[1]; //revisar
    paquete[5] = x[0];

    int num_tarea = 210;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    paquete[6] = t[0];

    //falta pasar a bytes el IP y el puerto
}

void N_naranja::confirm_pos_ACK(char *paquete, int num_req, int num_ID){
    //De que depende el responder con el ID o con 0 aqui? creo que siempre se responde con el ID
    /*creo que en los metodos ACK el paquete deberia tener el mismo numero de request que el
     metodo al que le esta respondiendo e.g se manda un confirm_pos con num request 45 entonces se responde con un confirm_pos_ACK con el num_request 45 tambien*/

    char * r;
    r = reinterpret_cast<char*>(&num_req);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];

    char * x;
    x = reinterpret_cast<char*>(&num_ID);
    paquete[4] = x[1]; //revisar
    paquete[5] = x[0];

    int num_tarea = 211;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    paquete[6] = r[3];
    paquete[7] = r[2];
    paquete[8] = r[1];
    paquete[9] = r[0];

    paquete[10] = t[0];
}

void N_naranja::disconnect_ACK(char * ACK, int num_req, int num_ID){
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    ACK[0] = r[3];
    ACK[1] = r[2];
    ACK[2] = r[1];
    ACK[3] = r[0];

    char * x;
    x = reinterpret_cast<char*>(&num_ID);
    ACK[4] = x[1];
    ACK[5] = x[0];

    int num_tarea = 216;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    ACK[6] = t[0];

    //el cuerpo va vacio entonces hay que ver que poner para identificarlo como vacio
}

void N_naranja::remove(char * paquete, int num_ID){
    int num_req = rand();
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];
    char * x;
    x = reinterpret_cast<char*>(&num_ID);
    paquete[4] = x[1];
    paquete[5] = x[0];
    int num_tarea = 220;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    paquete[6] = t[0];
    //el cuerpo va vacio entonces hay que ver que poner para identificarlo como vacio
    borrar_ID(num_ID);
}

void N_naranja::remove_ACK(char * ACK, int num_ID){
    int num_req = rand();
    char * r;
    r = reinterpret_cast<char*>(&num_req);
    ACK[0] = r[3];
    ACK[1] = r[2];
    ACK[2] = r[1];
    ACK[3] = r[0];
    char * x;
    x = reinterpret_cast<char*>(&num_ID);
    ACK[4] = x[1];
    ACK[5] = x[0];
    int num_tarea = 221;
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    ACK[6] = t[0];
    //el cuerpo va vacio entonces hay que ver que poner para identificarlo como vacio
    borrar_ID(num_ID);
}

int N_naranja::encontrar_nombre(int nombre, vector<Nodos> * vecinos){
    int indice = -1;

    /**
     * Busca entre el grafo el nodo al que corresponde el nombre y guarda el índice
     */
    for (int k = 0; k < grafo_verdes.size(); k++){
        if (nombre == grafo_verdes[k].nombre){
            indice = k;
            k = grafo_verdes.size();
        }
    }

    /**
     * Si no existe el nombre retorna fallo
     */
    if (indice<0){
        return indice;
    }

    /**
     * Si no existe el nombre retorna fallo
     */
    if (indice<0){
        return indice;
    }

    /**
     * Recorre los vecinos del nodo y los inserta en el vector de vecinos
     */
    for (int j = 0; j < grafo_verdes[indice].vecinos.size(); j++){
        vecinos->push_back(grafo_verdes[indice].vecinos[j]);
    }

    /**
     * Retorna la cantidad de vecinos
     */
    return grafo_verdes[indice].vecinos.size();
}

void N_naranja::fill_header(char * paquete, int num_request, int i_c_r, int tarea_realizar){
    char * r;
    r = reinterpret_cast<char*>(&num_request);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];
    char * x;
    x = reinterpret_cast<char*>(&i_c_r);
    paquete[4] = x[1];
    paquete[5] = x[0];
    int num_tarea = 221;
    char * t;
    t = reinterpret_cast<char*>(&tarea_realizar);
    paquete[6] = t[0];
}

void N_naranja::fill_neighbour(char * paquete, int id, unsigned int ip, unsigned short puerto, int size){
    char * x;
    x = reinterpret_cast<char*>(&id);
    paquete[15] = x[1];
    paquete[16] = x[0];
    if (size==17){
        return;
    }
    
    x = reinterpret_cast<char*>(&ip);
    paquete[17] = x[3];
    paquete[18] = x[2];
    paquete[19] = x[1];
    paquete[20] = x[0];
    x = reinterpret_cast<char*>(&puerto);
    paquete[21] = x[1];
    paquete[22] = x[0];
}

void N_naranja::connect_ACK(vector<char*> * ACK, int num_ID, int num_request){
    vector<Nodos> vtr;
    encontrar_nombre(num_ID, &vtr);
    unsigned int ip;
    unsigned short port;
    int tarea = 224;
    for (int i = 0; i < vtr.size(); i++){
        int size = 2;
        int header_size = 15;
        size += header_size;
        if (-1 <vtr[i].puerto){
            size += 6;
            ip = inet_addr(vtr[i].IP);
            port = htons(vtr[i].puerto);
        }

        char ack_pack[size];

        fill_header(ack_pack,num_request,num_ID,tarea);

        fill_neighbour(ack_pack, num_ID, ip, port, size);

        ACK->push_back(ack_pack);
    }   
}
