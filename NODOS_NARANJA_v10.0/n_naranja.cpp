#include "n_naranja.h"

N_naranja::N_naranja(string archivo_grafo_verdes, string archivo_configuracion,int indice){
    llenar_grafo_verdes(archivo_grafo_verdes);
    configurar(archivo_configuracion);
    /*for (size_t i = 0; i < naranjas.size(); i++) {
      if(strcmp(naranjas[i].IP,IP) == 0){
        this->ID = naranjas[i].nombre;
        break;
      }
    }*/
    this->IP = naranjas[indice].IP;
    this->ID = naranjas[indice].nombre;
    this->puerto = naranjas[indice].puerto;
    //cout << "IP de este naranja " << this->IP<<endl;
    //cout << "puerto de este naranja " << this->puerto<<endl;
    srand(static_cast<unsigned int>(time(nullptr)));
    esperando_request_pos_ACK = 0;
}

N_naranja::~N_naranja(){
  for(int i = 0; i < naranjas.size(); i++){
    free(naranjas[i].IP);
  }
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
void N_naranja::request_pos(char paquete[15],int num_req,int num_ID, int num_prioridad,int num_tarea){
    //se elige el numero de request
    //int num_req = rand();
    //char dksdoksdko[15];
    for (int i = 0; i < 15; i++) {
      paquete[i] = '\0';
    }


    char * r;
    r = reinterpret_cast<char*>(&num_req);
    paquete[0] = r[3];
    paquete[1] = r[2];
    paquete[2] = r[1];
    paquete[3] = r[0];

    //se elige el nombre aleatorio ID
    //int num_ID = 0;
    while(1){
        cout << "request pos con num ID verde:" << num_ID <<endl;
        //num_ID= rand() + 65536 % 65535;
        if(!revisar_ID(num_ID)){
            char * x;
            x = reinterpret_cast<char*>(&num_ID);
            paquete[4] = x[1]; //revisar
            paquete[5] = x[0];

            char r[4];
            r[0] = x[0];
            r[1] = x[1];
            r[2] = 0;
            r[3] = 0;
            int * temp = reinterpret_cast<int*>(r);

            cout << "se manda request pos con num ID verde:" << *temp <<endl;
            ultimo_nombre_asignado = num_ID; //para luego responder si ese nombre fue asignado por este nodo
            break;
        }
    }
    char * t;
    t = reinterpret_cast<char*>(&num_tarea);
    paquete[6] = t[0];

    char * p = (char *)&num_prioridad;
    paquete[7] = p[1];
    paquete[8] = p[0];

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
    int ya_instanciado = 0;
    for(int i = 0; i < verdes_instanciados.size();i++){
      if(num_ID == verdes_instanciados[i]){
          ya_instanciado=1;
      }
    }
    if(num_ID == ultimo_nombre_asignado){
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
            int num_respuesta = 0;
            char * x;
            x = reinterpret_cast<char*>(&num_respuesta);
            ACK[4] = x[1];
            ACK[5] = x[0];
            //se compara el numero de nodo, hay que ver donde se asigna el numero de nodo
        }
    }else if(ya_instanciado){
      int num_respuesta = 0;
      char * x;
      x = reinterpret_cast<char*>(&num_respuesta);
      ACK[4] = x[1];
      ACK[5] = x[0];
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

int N_naranja::confirm_pos(char *paquete, int num_ID, int num_req, char * IP, int port){
    //esperando_request_pos_ACK = 0; //si se esta mandando un confirm_pos significa que ya no se esta esperando ningun request_pos_ACK
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

    unsigned int ip = inet_addr(IP);
    x = reinterpret_cast<char*>(&ip);
    paquete[15] = x[3];
    paquete[16] = x[2];
    paquete[17] = x[1];
    paquete[18] = x[0];

    unsigned short puerto = htons(port);
    x = reinterpret_cast<char*>(&puerto);
    paquete[19] = x[1];
    paquete[20] = x[0];

    verdes_instanciados.push_back(num_ID);
    return 21;
}

void N_naranja::confirm_pos_ACK(char *paquete, int num_req, int num_ID){
    //De que depende el responder con el ID o con 0 aqui? creo que siempre se responde con el ID
    /*creo que en los metodos ACK el paquete deberia tener el mismo numero de request que el
     metodo al que le esta respondiendo e.g se manda un confirm_pos con num request 45 entonces se responde con un confirm_pos_ACK con el num_request 45 tambien*/
    verdes_instanciados.push_back(num_ID);
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
    paquete[6] = t[0];

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

void N_naranja::remove_ACK(char * ACK, int num_ID, int num_req){

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

int N_naranja::encontrar_nombre(char * IP,int puerto, vector<Nodos> * vecinos, int numID){
    //int indice = -1;
    int ID = -1;

    /**
     * Busca entre el grafo el nodo al que corresponde el nombre y guarda el índice
     */
    int ind;



    for(int i = 0; i < grafo_verdes.size(); i++){
      if(grafo_verdes[i].nombre == numID){
        ind = i;
        copiar(IP,grafo_verdes[i].IP, strlen(IP));
        grafo_verdes[i].puerto = puerto;
        break;
      }
    }

    for (int i = 0; i < grafo_verdes.size(); i++) {
      cout << "vecinos.size " << grafo_verdes[i].vecinos.size();
      for(int j = 0; j < grafo_verdes[i].vecinos.size(); ++j){
        if((grafo_verdes[i].nombre ==  grafo_verdes[i].vecinos[j].nombre) && grafo_verdes[i].puerto){
          cout << "se instancia vecino" << grafo_verdes[i].nombre << " con IP " <<IP << ":" << puerto <<endl;
          grafo_verdes[i].vecinos[j].IP = IP;
          grafo_verdes[i].vecinos[j].puerto = puerto;
        }
      }
    }


    cout << "Existe el nodo\n";
    /**
     * Recorre los vecinos del nodo y los inserta en el vector de vecinos
     */
     cout << "grafo_verdes[ind].vecinos.size(): " <<grafo_verdes[ind].vecinos.size();
    for (int j = 0; j < grafo_verdes[ind].vecinos.size(); j++){
        cout << "vecinos de este guineo : " << grafo_verdes[ind].vecinos[j].nombre << endl;
        vecinos->push_back(grafo_verdes[ind].vecinos[j]);
    }

    /**
     * Retorna la cantidad de vecinos
     */
    //cout<< "no es encontrar nombre" <<endl;
    return ID;
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
    char * t;
    t = reinterpret_cast<char*>(&tarea_realizar);
    paquete[6] = t[0];
}

void N_naranja::fill_neighbour(char * paquete, int id, int ip,  short puerto, int size){
    char * r;
    r = reinterpret_cast<char*>(&id);
    paquete[15] = r[1];
    paquete[16] = r[0];

    if(size == 17){
      return;
    }
    char * x;
    x = reinterpret_cast<char*>(&ip);
    cout << "ip del vecino del verde que se esta haciendo " << ip << endl;
    paquete[17] = x[3];
    paquete[18] = x[2];
    paquete[19] = x[1];
    paquete[20] = x[0];
    char * t;
    t = reinterpret_cast<char*>(&puerto);

    paquete[21] = t[1];
    paquete[22] = t[0];


    char puerto_vecino[2];
		puerto_vecino[0] = paquete[22];
		puerto_vecino[1] = paquete[21];
		/*puerto_vecino[2] = '\0';
		puerto_vecino[3] = '\0';*/
		short * puerto_vecino_num = (short*)(&puerto_vecino);
		short puerto_num = *puerto_vecino_num;
    cout << "puerto del vecino del verde que se esta haciendo " << puerto_num << endl;
}

void N_naranja::connect_ACK(vector<request> * ACK, int puerto,char * IP, int num_request,int num_ID){
    cout << "ENTRA A CONNECT_ACK\n";
    vector<Nodos> vtr;
    encontrar_nombre(IP,puerto, &vtr,num_ID);

    //asignarle IP y
    int ip = -1;
    short port = -1;
    int tarea = 201;
    for (int i = 0; i < vtr.size(); i++){
        int size = 2;
        int header_size = 15;
        size += header_size;
        request req;
        int indi;
        /*for(int j =0 ; j < grafo_verdes.size(); j++){
          if((grafo_verdes[j].nombre == vtr[i].nombre) && (grafo_verdes[j].puerto)){
            indi = j;
          }
        }
        for(int j = 0; j < grafo_verdes[indi].vecinos.size(); j++){
          for(int w = 0; w < grafo_verdes.size();w++){
            if(grafo_verdes[w].nombre == grafo_verdes[indi].vecinos[j].nombre){
              copiar(grafo_verdes[w].IP,vtr[j].IP,strlen(grafo_verdes[w].IP));
              copiar(grafo_verdes[w].IP,grafo_verdes[indi].vecinos[j].IP,strlen(grafo_verdes[w].IP));
              vtr[j].puerto = grafo_verdes[w].puerto;
              grafo_verdes[indi].vecinos[j].puerto = grafo_verdes[w].puerto;
            }
          }
        }*/

        if (vtr[i].puerto){
            cout << "tengo un vecino instanciado " <<vtr[i].nombre << endl;
            size += 6;
            ip = inet_addr(vtr[i].IP);
            cout << "IP transformado" << endl;
            port = htons(vtr[i].puerto);
            req.port = puerto;
            cout<<"aqui1"<<endl;
        }
        cout<<"aqui2"<<endl;
        req.port = puerto;
        req.size = size;
        copiar(IP, req.IP,strlen(IP));
        fill_header(req.paquete,num_request,num_ID,tarea);
        cout<<"aqui3"<<endl;
        fill_neighbour(req.paquete, vtr[i].nombre, ip, port, req.size);

        ACK->push_back(req);
    }

}

void N_naranja::llenar_grafo_verdes(string archivo_grafos_verdes){
  fstream archivo;
  archivo.open(archivo_grafos_verdes,ios::in);
  int cantidad_lineas = count(istreambuf_iterator<char>(archivo),istreambuf_iterator<char>(), '\n');
  //cout << cantidad_lineas << endl;
  archivo.close();
  archivo.open(archivo_grafos_verdes,ios::in);
  string linea,palabra;
  vector<string> columnas;
  for(int i = 0;i < cantidad_lineas; i++){
  //string temp;
  //while(archivo >> temp){
    columnas.clear();
    getline(archivo,linea);
    //cout << "linea" << linea << endl;
    stringstream s(linea);
    while(getline(s,palabra,',')){
      columnas.push_back(palabra);
    }
    N_verde nodo;
    //cout << "jjj" << columnas.size();
    for(int j = 0; j < columnas.size(); j++){
      if(!j){
        nodo.nombre = stoi(columnas[j]);
      }else{
        Nodos vecino;
        //cout << "hola " << stoi(columnas[j]);
        vecino.nombre = stoi(columnas[j]);
        nodo.vecinos.push_back(vecino);
      }
    }
    grafo_verdes.push_back(nodo);
  }
  archivo.close();
}

void N_naranja::copiar(char * src, char * dest, int tam){
  for(int i = 0 ; i < 15 ; i++){
    dest [i] = '\0';
  }
  for (int i = 0; i < tam; i++) {
    dest[i] = src[i];
  }
}

int N_naranja::configurar(string archivo_configuracion)
{

  //vector <nodo> data;
    int counter = 0;
    ifstream infile(archivo_configuracion);
    while (infile)
    {
      string s;
      if (!getline( infile, s ))
        break;

      istringstream ss( s );
      Nodos nodo;
      //char temp[s.size()+1];
      for(int i = 0; i < 2; ++i){
          string s;

          if (!getline( ss, s, ':' ))
            break;

          if(!i){
            nodo.IP = (char*)malloc(s.size()+1);
            strcpy(nodo.IP, s.c_str());
            /*cout<<"size s: "<<s.size()<<endl;
            copiar((char*)s.c_str(),nodo.IP,s.size());*/
            nodo.IP[s.size()] = '\0';
          //  cout << "ip sacado de archivo" << nodo.IP <<endl;
          }else{
            stringstream port(s);
            port >> nodo.puerto;
          //  cout << "port sacado de archivo" << nodo.puerto <<endl;
            nodo.nombre = counter;
          }
      }
      naranjas.push_back(nodo);
      ++counter;
    }

    if (!infile.eof())
    {
      cerr << "error!\n";
    }

    infile.close();
    /*for (int i = 0; i < 3; i++) {
      cout<<data.at(i).IP<<" "<<data.at(i).puerto<<" "<<data.at(i).nombre<<endl;
    }*/

    return 0;
}

int N_naranja::getID(){
  return this->ID;
}

vector<Nodos> N_naranja::getNaranjas(){
  return naranjas;
}

int N_naranja::getPuerto(){
    return this->puerto;

}
char * N_naranja::getIP(){
  return this->IP;
}

int N_naranja::getSizeGrafo(){
  return grafo_verdes.size();
}
int N_naranja::pedirNombreGrafo(int indice){
  return grafo_verdes[indice].nombre;
}
