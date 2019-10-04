#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <omp.h>
#include "cola.h"
#include <signal.h>
#define PORT	 8083
#define MAXLINE  512
#define WINDOWLEN 10
#define PACKAGE_SIZE 516

int sockfd;
char interfazSend_buff[MAXLINE];
int bandera_buffSend_lleno = 0; //0 si esta vacio y 1 si esta lleno
int bandera_buffSend_FA = 0; // 0 si aun no ha encontrado en Final de Archivo, 1 si ya finalizo
char interfazRecv_buff[MAXLINE];
int bandera_buffRecv_lleno = 0; //0 si esta vacio y 1 si esta lleno
int bandera_buffRecv_FA = 0; // 0 si aun no ha encontrado en Final de Archivo, 1 si ya finalizo
Cola cola_send(WINDOWLEN,PACKAGE_SIZE, 0);
Cola cola_recv(WINDOWLEN,MAXLINE, 0);
struct sockaddr_in servaddr, cliaddr;
int num_paquete = 0;
int ack = 0;

/*
Efecto:
	Si entra la señal enlazada y el socket continua abierto, lo cierra.
Requiere:
	Enlazar una señal a esta función.
Modifica:
	El descriptor de archivo del socket.
*/
void intHandler(int senal) {
	if(senal == SIGINT && sockfd){
		close(sockfd);
		exit(0);
	}
	exit(1);
}

/*
Efecto:
	Copia n elementos del buffer fuente en el de destino.
Requiere:
	Inicialización tanto del buffer fuente como del destino.
	No ingresar un tamaño inválido.
Modifica:
	El buffer destino.
*/
void copiar(char src[], char dest[], int size){//copia src a dest
		for(int i = 0; i < size; i++){
			dest[i] = src[i];
		}
}

/*
Efecto:
	Verifica si el buffer está vacío.
Requiere:
	El buffer inicializado.
Modifica:
	
*/
int esta_vacio(char buffer[MAXLINE]){//devuelve 0 si buffer no esta vacio y 1 si si
	for(int i = 0; i < MAXLINE; i++){
		if(buffer[i] != '\0'){
			return 0;
		}
	}

	return 1;
}

/*
Efecto:
	Envía hacia la interfáz los bloques a empaquetar.
Requiere:
	FILE inicializado.
Modifica:
	La interfáz.
	Bandera para activar envío
*/
void capa_Superior_Emisor(FILE * fd){
	int leido = 1;
    char buff_temp[MAXLINE];
		while(1){
			if(esta_vacio(interfazSend_buff))
				bandera_buffSend_lleno = 0;

		    if(!bandera_buffSend_lleno && !bandera_buffSend_FA){
		      leido = fread(&buff_temp,MAXLINE,1,fd);
			     if(leido > 0){
			         copiar(buff_temp,interfazSend_buff,MAXLINE);
			         bandera_buffSend_lleno = 1;
			         //++num_paquete;

			     }else {
						 	copiar(buff_temp,interfazSend_buff,MAXLINE);
						 	bandera_buffSend_lleno = 1;
			      	bandera_buffSend_FA = 1;
							break;
			    }
			}
		}
}

/*
Efecto:
	Actualiza la bandera del buffer de la interfáz.
Requiere:
	El buffer de la interfáz inicializado.
Modifica:
	Bandera de estado del buffer de la interfáz.
*/
void actualizar_bandera_interfaz_receptor(){
	if(esta_vacio(interfazRecv_buff)){
		bandera_buffRecv_lleno = 0;
	}else{
		bandera_buffRecv_lleno = 1;
	}
}

/*
Efecto:
	Elimina lo que hay en la interfáz.
Requiere:
	El buffer de la interfáz inicializado.
Modifica:
	El buffer de a interfáz
*/
void eliminar_elemento_interfaz(char interfaz_buff[MAXLINE]){//limpia el vector
	for(int i = 0; i < MAXLINE; i++){
		interfaz_buff[i] = '\0';
	}

}

/*
Efecto:
	Intenta insertar un dato en el bloque de la interfáz.
Requiere:
	Buffer de la interfáz inicializada.
Modifica:
	Buffer de la interfáz.
*/
//devuelve 1 si se inserto y cero si no se logro insertar
int insertar_interfaz_receptor(char elemento[MAXLINE]){
	if(!bandera_buffRecv_lleno && !esta_vacio(elemento)){
		copiar(elemento,interfazRecv_buff,MAXLINE);
		bandera_buffRecv_lleno = 1;
		return 1;
	}
	return 0;
}

/*
Efecto:
	Intenta sacar los datos de la interfáz.
Requiere:
	El buffer de la interfáz inicializado.
Modifica:
	El buffer de la interfáz.
*/
//saca de la interfaz el elemento que se le indique por medio de num_elemento y lo mete a parte_archivo. devuelve 1 si el elemento indicado no era vacio y 0 si si
int sacar_interfaz_receptor(char parte_archivo[MAXLINE]){
	if(bandera_buffRecv_lleno){
		copiar(interfazRecv_buff,parte_archivo,MAXLINE);
		eliminar_elemento_interfaz(interfazRecv_buff);
		bandera_buffRecv_lleno = 0;
		return 1;
	}
	return 0;
}

/*
Efecto:
	Coloca el header en el paquete.
Requiere:
	Buffer a empaquetar inicializado.
Modifica:
	El buffer a empaquetar.
*/
void set_header(int SNmin,char buff_a_empaquetar[PACKAGE_SIZE]){ //primer parametro es lo que retorna get_Nmin de cola. LLena los primeros 4 bytes del buffer que sera enviado finalmente
	char * p;
	p = (char *)&SNmin;
	buff_a_empaquetar[0] = '\0';
	buff_a_empaquetar[1] = p[2];
	buff_a_empaquetar[2] = p[1];
	buff_a_empaquetar[3] = p[0];

}

/*
Efecto:
	Crea el último paquete para cerrar la conexión.
Requiere:
	El buffer a empaquetar esté inicializado.
Modifica:
	El buffer a empaquetar.
*/
void crear_ultimo_paquete(char buff_a_empaquetar[PACKAGE_SIZE]){
	set_header(num_paquete,buff_a_empaquetar);
	buff_a_empaquetar[4] = '*';
	for(int i = 5; i < PACKAGE_SIZE; ++i){
		buff_a_empaquetar[i] = '\0';
	}

}

/*
Efecto:
	Sacar los datos del paquete y los mete en datos.
Requiere:
	Los dos buffer inicializados.
Modifica:
	El buffer donde se desean colocar los paquetes.
*/
void sacar_datos(char * paquete, char * buffer){//saca los datos del paquete buffer y los mete a datos
	for (int i = 0; i < MAXLINE; i++) {
		paquete[i] = buffer[i+4];
	}
}

/*
Efecto:
	Mete los datos del buffer en el paquete.
Requiere:
	Ambos buffers inicializados.
Modifica:
	El buffer paquete.
*/
void meter_datos(char * paquete, char * buffer){
	for (int i = 0; i < MAXLINE; i++) {
		paquete[i+4] = buffer[i];
	}
}

/*
Efecto:
	Empaqueta el buffer.
Requiere:
	El buffer inicializado.
Modifica:
	El buffer a empaquetar.
*/
void empaquetar(char * buff_a_empaquetar, char * datos){
  set_header(num_paquete,buff_a_empaquetar);
  meter_datos(buff_a_empaquetar,datos);
}

/*
Efecto:
	Intenta meter los datos de la interfáz en la cola.
Requiere:
	El buffer de la interfáz y la cola inicializada correctamente.
Modifica:
	La cola.
*/
int poner_en_colaSend(){//retorna si se logro meter en la cola
	int put = 0;
	if(bandera_buffSend_lleno){

		char buff_empaquetado[PACKAGE_SIZE];
		empaquetar(buff_empaquetado,interfazSend_buff);
		put = cola_send.put(num_paquete, buff_empaquetado,PACKAGE_SIZE);
		if(put!=-1){
			eliminar_elemento_interfaz(interfazSend_buff);
			bandera_buffSend_lleno = 0;
      num_paquete++;
		}
	}

	return put;
}

/*
Efecto:
	Obtiene el header del paquete.
Requiere:
	El buffer de paquete inicializado.
Modifica:
	El buffer header.
*/
void get_Header(char* paquete,char * header){
	header[0] = paquete[3];
	header[1] = paquete[2];
	header[2] = paquete[1];
	header[3] = 0;

}

/*
Efecto:
	Se obtiene el paquete que está en la cola y se envía.
Requiere:
	La cola inicializada.
	Conexión con el socket establecida.
Modifica:
	
*/
void enviar(){
  char buff_temp[PACKAGE_SIZE];
  int i;
  i = cola_send.get_nMin();
  while(cola_send.get_send(buff_temp,i,PACKAGE_SIZE) ){
    	sendto(sockfd, (const char *)buff_temp, PACKAGE_SIZE, MSG_CONFIRM , (const struct sockaddr *) &cliaddr, sizeof(cliaddr));
    	usleep(1);
      ++i;
  }
}

/*
Efecto:
	Envía ack del paquete que se necesita.
Requiere:
	Cola inicializada.
	Conexión con el socket establecida.	
Modifica:
	
*/
void mandar_ack(char* ip, int port){
	char paquete_ack[PACKAGE_SIZE];
	paquete_ack[0] = (char)1;
	char * encabezado_ack;
	int rn = cola_recv.get_nMin();
	if(cola_recv.esta_vacio(cola_recv.get_nMin()%WINDOWLEN)){
		encabezado_ack = (char*)(&rn);//pasa el numero a tres bytes(los del encabezado del ack)
		paquete_ack[1] = encabezado_ack[2];
		paquete_ack[2] = encabezado_ack[1];
		paquete_ack[3] = encabezado_ack[0];//mete los tres bytes al encabezado del paquete
		sendto(sockfd, (const char *)paquete_ack, PACKAGE_SIZE, MSG_CONFIRM , (const struct sockaddr *) &cliaddr, sizeof(cliaddr));//manda el paquete con el ack
	}
}

/*
Efecto:
	Verifica si el paquete es el último en la conexión.
Requiere:
	El buffer inicializado.
Modifica:
	
*/
int revisar_si_es_ultimo_paquete(char paquete[MAXLINE]){//devuelve 0 si no es el ultimo paquete, 1 si si es
	if(paquete[0] == '*'){
		for(int i = 1; i < MAXLINE; i++){
			if(paquete[i]!='\0'){
				return 0;
			}
		}
		return 1;
	}
	return 0;
}



int main() {
	FILE *fptr;
	//se abre el archivo con permisos de escritura y lectura binarios
  //struct sockaddr_in servaddr, cliaddr;
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;

  socklen_t len;
	int tipo = -1; //si tipo = 0  recibe, si tipo = 1 manda

	int signal_file_abierto = 0;
	int ip_asignado = 0;
	eliminar_elemento_interfaz(interfazSend_buff); //Inicializa la interfaz en 0s
	eliminar_elemento_interfaz(interfazRecv_buff); //Inicializa la interfaz en 0s
	char archivo_resultado[64];
	int nombre_asignado = 0;
	int salir = -500;
	#pragma omp parallel num_threads(7) shared(sockfd,cliaddr,fptr,interfazSend_buff,bandera_buffSend_lleno,bandera_buffSend_FA,interfazRecv_buff,\
	bandera_buffRecv_lleno,bandera_buffRecv_FA,cola_send,cola_recv,ack,signal_file_abierto,ip_asignado,archivo_resultado,nombre_asignado, salir)
	{
		signal(SIGINT, intHandler);

		if (omp_get_thread_num()==0) {//hilo compartido por receptor y emisor

			char archivo[64];
			char buffIP[15];
			int port;
			printf("Digite 0 si va a recibir un archivo, digite 1 si va a mandar un archivo\n");
			scanf("%5d", &tipo);
			if(tipo == 1){ //si es de tipo mandar archivo la direccion se guarda en cliaddr
				printf("Digite el ip, el puerto y el archivo que desea mandar, en ese orden, separados por espacio\n");
				scanf("%s %d %[^\n]s",buffIP,&port,archivo);
				nombre_asignado =1;
				cliaddr.sin_family = AF_INET;
				cliaddr.sin_port = htons(port);
				cliaddr.sin_addr.s_addr = inet_addr(buffIP);
				servaddr.sin_port = htons(PORT);
				ip_asignado = 1;
				if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
				{
					perror("bind failed");
					exit(EXIT_FAILURE);
				}
				fptr = fopen(archivo,"rb+");

				if (fptr == NULL) {
					printf("El archivo no existe.\n");
					exit(0);
				}else{
					signal_file_abierto = 1;
				}

			}else if(tipo == 0){
				int puerto;
				printf("Digite el numero de puerto que desea asignar y el nombre del archivo que se va a generar, separados por espacio: ");
				scanf("%6d %[^\n]s",&puerto,archivo_resultado);
				nombre_asignado = 1;
				servaddr.sin_port = htons(puerto);
				printf("El puerto asignado es %d, esperando a recibir un archivo.\n", puerto);

				if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 )
				{
					perror("bind failed");
					exit(EXIT_FAILURE);
				}

			}else{
				printf("Opcion invalida.\n");
				exit(0);
			}
			while(1){
				char paquete_temporal[PACKAGE_SIZE];
				int bytes_recibidos = recvfrom(sockfd, (char *)paquete_temporal, PACKAGE_SIZE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, &len);
				ip_asignado = 1;
				if(paquete_temporal[0] == '\0'){//es un paquete de informacion
					#pragma omp critical (receptor)
					{
						char header[4];
						char datos[MAXLINE];
						get_Header(paquete_temporal,header);
						/*se convierte el header a numero*/
						int * x = (int*)(&header);
						int numero_de_paquete = *x;
						sacar_datos(datos,paquete_temporal);
						cola_recv.put(numero_de_paquete,datos,MAXLINE); //mete el paquete en la posicion numero_de_paquete%10 de la cola
					}
					usleep(4);
				}else{//es un ack
					char header[4];
					get_Header(paquete_temporal,header);
					int * numero_de_ack = (int*)(&header);
					ack = *numero_de_ack;
          cola_send.set_nMin(ack);
				}
			}
		}else if(omp_get_thread_num()==1 ){//segundo hilo de receptor
			//continua sacando elementos de la cola y metiendolos al buffer de la interfaz hasta encontrar un elemento que no esta
			while(1){
				#pragma omp critical (receptor)
				{
					if(!tipo && ip_asignado){
							if(omp_get_thread_num() == 1){

								char elemento_cola[MAXLINE];
								if(cola_recv.get_recv(elemento_cola,MAXLINE)){ //si se encontraba el paquete en la cola
									usleep(3);
                  int repetir = 1;
                  while(repetir){
                    if(insertar_interfaz_receptor(elemento_cola)){
                      repetir = 0;
                      cola_recv.set_RNmin(cola_recv.get_nMin()+1);
                    }
                  }
								}
							}
						}
					}
					usleep(4);
				}
			}else if(omp_get_thread_num() == 2){
				while(1){
					if(nombre_asignado){
					FILE* fptr1;
					fptr1 = fopen(archivo_resultado, "ab+");//se abre para modificarlo
					while(1){
						actualizar_bandera_interfaz_receptor();

						if(bandera_buffRecv_lleno){//si no esta vacio el elemento
							char parte_archivo[MAXLINE];
							sacar_interfaz_receptor(parte_archivo);//Pone la bandera de lleno en 0

							if(!revisar_si_es_ultimo_paquete(parte_archivo)){//si el dato no es un asterisco
								fwrite(parte_archivo,sizeof(parte_archivo),1,fptr1);//se mete en el archivo
							}else{
								char * ipstr = inet_ntoa(cliaddr.sin_addr);
								int port = ntohs(cliaddr.sin_port);
								mandar_ack(ipstr,port);
								bandera_buffRecv_FA = 1;
								fclose(fptr1);//se termino el archivo
								printf("%s\n","Archivo recibido");
								exit(0);
							}
						}
							usleep(4);
					}
				}
			}
		}else if(omp_get_thread_num() == 3){//hilo de receptor / Capa Superior
			while(1){

				if(!tipo && ip_asignado){
					usleep(6);//espera 6 microsegundos y manda ack
					char * ipstr = inet_ntoa(cliaddr.sin_addr);
					int port = ntohs(cliaddr.sin_port);
					mandar_ack(ipstr,port); //manda el ack
				}

			}
		}else if(omp_get_thread_num() == 4 || omp_get_thread_num() == 5){//hilo de emisor
			int stop = 0;

			while(1){

				#pragma omp critical (emisor)
				{

					if(tipo == 1){
						if(omp_get_thread_num() == 4 && signal_file_abierto && !stop){
							poner_en_colaSend();
							if(bandera_buffSend_FA){
								poner_en_colaSend();
								char paquete_final[PACKAGE_SIZE];
								crear_ultimo_paquete(paquete_final);//paquete con el '*'
								salir = num_paquete;
								stop = cola_send.put(num_paquete, paquete_final,PACKAGE_SIZE); //caso en el que la cola este llena, se queda intentando meter el ultimo paquete hasta que haya espacio
							}

						}else if(omp_get_thread_num() == 5 && ip_asignado){
							enviar();
							if(ack == salir+1){
								exit(0);
							}
							usleep(4);
						}
					}
				}
				usleep(7);
			}

		}else if(omp_get_thread_num() == 6){//hilo de emisor/ Capa Superior
			while(1){
				if(tipo == 1){
					if(signal_file_abierto){
						capa_Superior_Emisor(fptr);
						fclose(fptr);
						break;
					}
				}
			}
		}
  }
  return 0;
}
