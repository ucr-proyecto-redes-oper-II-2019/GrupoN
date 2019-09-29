#include "cola.h"
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <omp.h>
#include <netinet/in.h>
#include <signal.h>
/*
backlog:
    historias que fueron votadas con la cantidad de horas asignadas
    y un puntaje para saber la cantidad de tareas que puede realizar
    una persona por semana. Estas historias se listan para poder hacer
    esto durante un sprint.
standup:
    se habla un poco algo rápido de el estado que tiene cada integrante
    y si está bloqueado con alguna historia.
sprint:
    trabajos que se van realizando por semanas.
go-review:
    un pre analisis antes de que se suba a git.
*/

#define PORT	 8085
#define PACKAGE_SIZE 4


/*
VARIABLES PARA EL SOCKET
*/
int sockfd;
struct sockaddr_in emisor, envio, receptor, recibo;
char ip[15];
int port;


/*
VARIABLES DEL PROGRAMA
*/
int maxline = 0;
int windowlen = 10;
int total_size = 0;
char * buff_inter;
Cola * cola;
int buff_filled = 0;
int connected = 1;
int end_connect = 0;
int tipo = 1;
int siga = 1;


/*
BLOQUE DE PROTOTIPOS
*/
void clear(char * dest, int size);
void init_var_connection();
int get_cola_send_pack();
void receive_from_interface();
int receive_msj();
int put_in_interface();
int send_ack();
int recv_conection(int size_max, int size_window, int type_connection, int puerto);
int send_connection(int size_max, int size_window, int type_connection,char * buffIP,int port);
int empaquetar(char * temp);
int connection_finished();
int end_connection(); // hilo conecta/desconecta y envía
void iniciar_receptor(int puerto);
void iniciar_emisor(char * n_ip,int n_port);
int get_from_interface(char * src);
int send_to_interface(char * src); // hilo conecta y envía a interfaz
int get_cola_send_pack();//hilo saca de cola y envía el paquete
int desempaquetar(char * dest_C, int * dest_I, char * src);
int comprueba_ack(char * paquete);


void handler_function (int parameter){
    if (sockfd && parameter==SIGINT) {
        close(sockfd);
    }
    if (buff_inter && parameter==SIGINT) {
        delete buff_inter;
    }
    if (cola && parameter==SIGINT) {
        delete cola;
    }
    exit(1);
}

int enviar(char * ip, int puerto, int win_size, int archivo_len,FILE * archID){
    signal(SIGINT,(handler_function));
#pragma omp parallel num_threads(4) shared(maxline,windowlen,total_size,connected,cola,buff_inter,buff_filled,end_connect,tipo,siga,emisor,envio,receptor,recibo)
{
    if (omp_get_thread_num()==0) {
        get_cola_send_pack(); // se activa con la posición 1
    } else if (omp_get_thread_num()==1) {
        receive_from_interface();
    } else if (omp_get_thread_num()==2) {
        receive_msj();
    } else {
        send_connection(archivo_len,win_size,1,ip,puerto);
        int seguir = 0;
        char buffer[archivo_len];
        seguir = fread(&buffer,sizeof(buffer),1,archID);
        while (seguir) {
            send_to_interface(buffer);
            seguir = fread(&buffer,sizeof(buffer),1,archID);
        }
        end_connection();
        fclose(archID);
    }
}
    return 0;
}

int recibir(int win_size, int archivo_len,int puerto,FILE * archID){
        signal(SIGINT,handler_function);
#pragma omp parallel num_threads(4) shared(maxline,windowlen,total_size,connected,cola,buff_inter,buff_filled,end_connect,tipo,siga,emisor,envio,receptor,recibo)
{
    if (omp_get_thread_num()==1) {
        put_in_interface();
    } else if (omp_get_thread_num()==2) {
        send_ack();
    } else if (omp_get_thread_num()==3) {
        receive_msj();
    } else {
        recv_conection(archivo_len,win_size,0,puerto);
        int seguir = 0;
        char buffer[archivo_len];
        while (!connection_finished()) {
            get_from_interface(buffer);
            seguir = fwrite(&buffer,sizeof(buffer),1,archID);
        }
        fclose(archID);
    }
}
    return 0;
}


// hilo recibe ack y actualiza cola


/*
BLOQUE CONEXION
*/
int send_connection(int size_max, int size_window, int type_connection,char * buffIP,int port){
    tipo = type_connection;
    maxline = size_max;
    windowlen = size_window;
    total_size = maxline+PACKAGE_SIZE;
    init_var_connection();
    iniciar_emisor(buffIP,port);
    /*
    siga = 0;
    while (!connected){
        sendto(sockfd, (const char *)buff_inter, strlen(buff_inter), MSG_CONFIRM , (const struct sockaddr *) &envio, sizeof(envio));
        printf("ENVIANDO DE NUEVO EL PAQUETE\n");
        sleep(1);
    }
    end_connect = 0;
    printf("PAQUETE DE CONEXION RECIBIDO\n");
    */
    return 0;
}

int recv_conection(int size_max, int size_window, int type_connection, int puerto){
    tipo = type_connection;
    maxline = size_max;
    windowlen = size_window;
    total_size = maxline+PACKAGE_SIZE;
    init_var_connection();
    iniciar_receptor(puerto);
    /*
    while (!connected) {
        char paquete[total_size];
        socklen_t len;
        printf("ESPERO LA CONEXION\n");
        recvfrom(sockfd, (char *)paquete, maxline, MSG_WAITALL, ( struct sockaddr *) &recibo, &len);

        if (paquete[0]==1 && paquete[PACKAGE_SIZE]=='*') {
            char * t_ip = inet_ntoa(recibo.sin_addr);
            cola->copy_to_others(ip,t_ip,15,0,0);
            port = ntohs(recibo.sin_port);
            connected = 1;
            siga = 0;
            end_connect = 0;
            printf("RECIBÍ UN MSJ\n");
        }
    }
    */
    return 0;
}

int end_connection(){
    end_connect = 1;
    return 0;
}

int connection_finished(){
    if (end_connect) {
        return 1;
    }
    return 0;
}

/*
BOLQUE DE RECEPTOR
*/
int get_from_interface(char * src){
    while (!buff_filled)
        usleep(1);
    cola->copy_to_others(buff_inter,src,maxline,0,0);
    clear(buff_inter,maxline);
    buff_filled = 0;
    return 0;
}

int put_in_interface(){
    while (!connected)
        usleep(1);
    while (!end_connect | cola->get_counter()) {
        while (buff_filled)
            usleep(1);
        if (cola->get_recv(buff_inter,maxline)) {
            buff_filled = 1;
        }
    }
    return 0;
}

int receive_msj(){
    /*
    while (siga)
        usleep(1);
    */
    while (!end_connect) {
        socklen_t len;
        struct sockaddr_in temp;
        char ptemp[total_size];
        recvfrom(sockfd, (char *)ptemp, maxline, MSG_WAITALL, ( struct sockaddr *) &temp, &len);
        if (temp.sin_addr.s_addr == recibo.sin_addr.s_addr && temp.sin_port == recibo.sin_port) {
            int ack = comprueba_ack(ptemp);
            if (!ack && !tipo) {
                if (ptemp[PACKAGE_SIZE]=='*') {
                    end_connect=1;
                } else {
                    char datos[maxline];
                    int num_paquete;
                    desempaquetar(datos,&num_paquete,ptemp);
                    cola->put(num_paquete,ptemp,maxline);
                }
            } else if (tipo) {
                char datos[maxline];
                int num_paquete;
                desempaquetar(datos,&num_paquete,ptemp);
                if (datos[0]==0 && datos[PACKAGE_SIZE=='*']) {
                    connected=0;
                }
                if (num_paquete==0) {
                    connected=1;
                } else {
                    cola->set_nMin(num_paquete);
                }
            }
        }
    }
    return 0;
}

int desempaquetar(char * dest_C, int * dest_I, char * src){
    cola->copy_to_others(dest_C, src, maxline, 0, PACKAGE_SIZE);
    dest_I[0] = src[1];
    dest_I[1] = src[2];
    dest_I[2] = src[3];
    dest_I[3] = 0;
    return 0;
}

int send_ack(){
    while (!connected)
        usleep(1);
    while (!end_connect) {
        int ack = cola->get_nMin();
        char * k = (char*)&ack;
        char temp[total_size];
        clear(temp,total_size);
        cola->copy_to_others(temp,k,3,1,0);
        sendto(sockfd, (const char *)temp, strlen(temp), MSG_CONFIRM , (const struct sockaddr *) &recibo, sizeof(recibo));
        usleep(10);
    }
    int i = 100;
    char temp[total_size];
    clear(temp,total_size);
    temp[PACKAGE_SIZE]='*';
    while (i) {
        i--;
        sendto(sockfd, (const char *)temp, strlen(temp), MSG_CONFIRM , (const struct sockaddr *) &recibo, sizeof(recibo));
    }
    return 0;
}

// si retorna un 0 es un ack sino es un paquete
int comprueba_ack(char * paquete){
    if (paquete[0]==0) {
        return 1;
    }
    return 0;
}

/*
BLOQUE DE EMISOR
*/

int get_cola_send_pack(){
    while (!connected)
        usleep(10);
    while (!end_connect | cola->get_counter()) {
        int final = cola->get_size() + cola->get_nMin();
        for (int i = cola->get_nMin(); i < final; i++) {
            char temp[total_size];
            if (cola->get_send(temp,i,total_size)) {
                sendto(sockfd, (const char *)temp, strlen(temp), MSG_CONFIRM , (const struct sockaddr *) &envio, sizeof(envio));
                printf("PAQUETE ENVIADO : %d\n", i);
            }
        }
    }
    char temp[total_size];
    empaquetar(temp);
    while (connected) {
        sendto(sockfd, (const char *)temp, strlen(temp), MSG_CONFIRM , (const struct sockaddr *) &envio, sizeof(envio));
        usleep(1);
    }
    return 0;
}

int send_to_interface(char * src){
    while (buff_filled | !connected)
        usleep(10);
    cola->copy_to_others(buff_inter,src,maxline,0,0);
    buff_filled = 1;
    return 0;
}

void receive_from_interface(){
    while (!connected)
        usleep(1);
    while (!end_connect) {
        while (!buff_filled)
            usleep(1);
        char temp[total_size];
        int num_package = empaquetar(temp);
        cola->put(num_package,temp,total_size);
        clear(buff_inter,maxline);
        buff_filled = 0;
    }
}

int empaquetar(char * dest){
    cola->copy_to_others(dest,buff_inter,maxline,PACKAGE_SIZE,0);
    int ultimo = 1 + cola->get_last();
    char * num_package = (char*)&ultimo;
    char k[3];
    cola->copy_to_others(k,num_package,3,0,0);
    k[0] = num_package[0];
    k[1] = num_package[1];
    k[2] = num_package[2];
    cola->copy_to_others(dest,k,3,1,0);
    dest[0]=1;
    return ultimo;
}



/*
BLOQUE EXTRA
*/
void iniciar_emisor(char * n_ip,int n_port){
    port = n_port;
    cola->copy_to_others(ip,n_ip,15,0,0);
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    memset(&emisor, 0, sizeof(emisor));
    memset(&envio, 0, sizeof(envio));

	emisor.sin_family = AF_INET; // IPv4
	emisor.sin_addr.s_addr = INADDR_ANY;
	emisor.sin_port = htons(PORT);
    if ( bind(sockfd, (const struct sockaddr *)&emisor, sizeof(emisor)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	envio.sin_family = AF_INET; // IPv4
	envio.sin_port = htons(port);
	envio.sin_addr.s_addr = inet_addr(ip);
}

void iniciar_receptor(int puerto){
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    memset(&receptor, 0, sizeof(receptor));

	receptor.sin_family = AF_INET; // IPv4
	receptor.sin_addr.s_addr = INADDR_ANY;
	receptor.sin_port = htons(puerto);

    if ( bind(sockfd, (const struct sockaddr *)&receptor, sizeof(receptor)) < 0 )
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
}

void init_var_connection(){
    char * temp=new char[maxline];
    buff_inter = temp;
    //clear(buff_inter, total_size);
    buff_filled=0;
  //  buff_inter[0]=1;
  //  buff_inter[PACKAGE_SIZE]='*';
    printf("sale\n");
    cola = new Cola(windowlen,1);
    printf("sale\n");
}

void clear(char * dest, int size){//limpia dest
	for(int i = 0; i < size; i++){
		dest[i] = 0;
	}
}
