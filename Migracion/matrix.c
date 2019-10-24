#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include<signal.h>

  FILE *fptr;//puntero del archivo continue
/* ----- Variables globales ----------- */
  int dimCopy, nCopy, iCopy, jCopy, itersCopy, totalprodCopy;
  double ACopy[15][15], BCopy[15][15], ICopy[15][15], TempCopy[15][15], detCopy, sdetCopy, cCopy;
  int caso; //variable para determinar de donde se va a reanudar (en cual etiqueta) Caso 1 = goto savePoint1, Caso 2 = goto savePoint2, Caso 3 = goto savePoint3
  int bandera;

/* ----- Subrutinas y funciones ----------- */
void mult(double X[15][15], double Y[15][15], double Temp[15][15], int dim) {
	int i, j, k;
	for (i=0; i<dim; i++)
	  for (j=0; j<dim; j++) {
		Temp[i][j] = 0;
		for (k=0; k<dim; k++)
		  Temp[i][j] += X[i][k]*Y[k][j];
	  }
	for (i=0; i<dim; i++)
	  for (j=0; j<dim; j++) {
        X[i][j] = Temp[i][j];
	}
}
/* -----------------*/

void lea(FILE *fd, double X[15][15], int dim) {
	int i, j;
	for (i=0; i<dim; i++) {
	  for (j=0; j<dim; j++)
         fscanf(fd, "%lf", &X[i][j]);
	 }
}
/* -----------------*/

void imprima(double X[15][15], int dim) {
	int i, j;
	for (i=0; i<dim; i++) {
	  for (j=0; j<dim; j++)
         printf("%12.2lf", X[i][j]);
      printf("\n");
	 }
}
/* -----------------*/

void ident(double X[15][15], int dim) {
	int i, j;
	for (i=0; i<dim; i++) {
	  for (j=0; j<dim; j++)
        if (i == j) X[i][i] = 1.0;
        else X[i][j] = 0.0;
	 }
}
/* -----------------*/

void scalar(double X[15][15], int dim, double val) {
	int i, j;
	for (i=0; i<dim; i++) {
	  for (j=0; j<dim; j++)
        X[i][j] *= val;
	 }
}
/* -----------------*/

int verify(double X[15][15], int dim) {
	int i, j;
	for (i=0; i<dim; i++) {
	  for (j=0; j<dim; j++)
        if (i == j) {
			if (X[i][i] != 1.0)
			   return(0);
		}
		else {
		  if (X[i][j] != 0.0)
		     return(0);
		 }
	 }
	 return(1);
}

/* -----------------*/

void copiarMatriz(double ACopy[15][15], double A[15][15]){
	for(int i = 0; i < 15; i++){
		for(int j = 0; j < 15; j++){
			ACopy[i][j] = A[i][j];
		}
	}
}

/* -----------------*/

void guardar(double A[15][15], double B[15][15], double I[15][15], double Temp[15][15], double det, double sdet, double c,int dim, int n, int i, int j, int iters, int totalprod ){
	dimCopy = dim;
	nCopy = n;
	iCopy = i;
	jCopy = j;
	itersCopy = iters;
	totalprodCopy = totalprod;
	copiarMatriz(ACopy,A);
	copiarMatriz(BCopy,B);
	copiarMatriz(ICopy,I);
	copiarMatriz(TempCopy,TempCopy);
	detCopy = det;
	sdetCopy = sdet;
	cCopy = c;
}

/* -----------------*/

void sig_handler(int signo)
{
  if (signo == SIGINT)
    bandera = 1;
}

void escribirMatrizEnArchivo(double matriz[15][15]){
  int i
  for(i = 0; i < 15; i++){
    for(j = 0; j < 15; j++){
      fprintf(fptr,"%f ", matriz[i][j]);
    }
  }
  fprintf(fptr,"\n");
}
/* -----------------*/
/*
int dimCopy, nCopy, iCopy, jCopy, itersCopy, totalprodCopy;
double ACopy[15][15], BCopy[15][15], ICopy[15][15], TempCopy[15][15], detCopy, sdetCopy, cCopy;
int caso; //variable para determinar de donde se va a reanudar (en cual etiqueta) Caso 1 = goto savePoint1, Caso 2 = goto savePoint2, Caso 3 = goto savePoint3
int bandera;
*/
void escribirArchivo(){
	//escribir todas las variables globales en archivo continue + variable caso
  fprintf(fptr,"%d ",dimCopy);
  fprintf(fptr,"%d ",nCopy);
  fprintf(fptr,"%d ",iCopy);
  fprintf(fptr,"%d ",jCopy);
  fprintf(fptr,"%d ",itersCopy);
  fprintf(fptr,"%d ",totalprodCopy);
  fprintf(fptr,"%f ",detCopy);
  fprintf(fptr,"%f ",sdetCopy);
  fprintf(fptr,"%f ",cCopy);
  fprintf(fptr,"%d ",caso);
  fprintf(fptr,"%d\n",bandera);
  escribirMatrizEnArchivo(ACopy);
  escribirMatrizEnArchivo(BCopy);
  escribirMatrizEnArchivo(ICopy);
  escribirMatrizEnArchivo(TempCopy);
	//exist(); (supongo que es exit)
}

void leerDeMatrizEnArchivo(double matriz[15][15]){
  for(int i = 0 ; i < 15 ; i++){
    for(int j = 0; j < 15; j++){
      fscanf("%f ",matriz[i][j]);
    }
  }
}
void cargarVariables(){
  fscanf("%d %d %d %d %d %d %f %f %f %d %d\n", &dimCopy,&nCopy,&iCopy,&jCopy,&itersCopy,&totalprodCopy,&detCopy,&sdetCopy,&cCopy,&caso,&bandera);
  leerDeMatrizEnArchivo(ACopy);
  leerDeMatrizEnArchivo(BCopy);
  leerDeMatrizEnArchivo(ICopy);
  leerDeMatrizEnArchivo(TempCopy);
}
/* --------------------------MAIN---------------------------------*/
int main(void) {
/* ----- Variables -----*/
    FILE *fdata, *fout;
    int dim, n, i, j, iters, totalprod;
    double A[15][15], B[15][15], I[15][15], Temp[15][15], det, sdet, c;
    bandera = 0;
    caso = 0;

/* --- Instrucciones ---*/
    fdata = fopen("matrices.dat", "r");
    if( fdata == NULL ) {
        perror("Error opening matrices.dat: ");
        return(-1);
    }

    fout  = fopen("trace.txt", "w");
    if( fout == NULL ) {
        perror("Error opening trace.txt: ");
        return(-1);
    }
    fclose(fout);

    //ARCHIVO PARA REANUDAR
    //FILE *fptr;
    //se abre el archivo con permisos de escritura y lectura, file must exist
    fptr = fopen("continue.dat","r+");

    //el archivo existe
    if(fptr != NULL){
        //CARGAR LAS VARIABLES CON LO QUE SE LEE DEL ARCHIVO
        cargarVariables();
        //extraer tambien el valor de la variable "caso" (va a indicar a cual etiqueta saltar para reanudar)
        switch(caso){
        case 1: goto savePoint1;
            break;
        case 2: goto savePoint2;
            break;
        case 3: goto savePoint3;
            break;
        }

    }//si no existe no hace falta un else, va directo a ejecutar las lineas que le siguen, en caso de que si existia ya salto a donde queria

    //se abre de nuevo con permisos de escritura y lectura pero esta vez crea el archivo
    fptr = fopen("continue.dat","w+");

    fscanf(fdata, "%d %lf", &dim, &det);
    lea(fdata, A, dim);
    lea(fdata, B, dim);
    fclose(fdata);

    sdet = sqrt(det);
    c = 1.0/sdet;
    srand(time(0));
    printf("Leidos: dim=%d, det=%lf, sdet=%lf\n", dim, det, sdet);
    printf("\nMatriz A leida:\n");
    imprima(A, dim);

    printf("\nMatriz B leida:\n");
    imprima(B, dim);

    ident(I, dim);

    iters = 0;
    totalprod = 0;

    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");

    while (1) {
        fout  = fopen("trace.txt", "a");
        if( fout == NULL ) {
            perror("Error opening trace.txt: ");
            return(-1);
        }
        n = rand() % 6 + 1;
        for (i=0; i<n; i++) {
            mult(I, A, Temp, dim);
            scalar(I, dim, c);
            if(bandera) {
                escribirArchivo();
                guardar(A, B, I, Temp, det, sdet, c, dim, n, i, j, iters, totalprod);
                caso = 1;
                exit();
            }
savePoint1:
        }


        for (i=0; i<n; i++) {
            mult(I, B, Temp, dim);
            scalar(I, dim, c);
            if(bandera) {
                escribirArchivo();
                guardar(A, B, I, Temp, det, sdet, c, dim, n, i, j, iters, totalprod);
                caso = 2;
                exit();
            }
savePoint2:
        }

        if (verify(I, dim)) {
            iters++;
            totalprod += 2*n;
            fprintf(fout, "Iteracion %d verificada OK: productos = %d, \ttotalprod = %d\n",
            iters, 2*n, totalprod);
            printf("Completadas %d iteraciones\n", iters);
        }
        else {
            printf("Iter %d presenta error. Se cancela el programa\n", iters+1);
            fprintf(fout, "Iter %d presenta error. Se cancela el programa\n", iters+1);
            exit(1);
        }


        if(bandera) {
            escribirArchivo();
            guardar(A, B, I, Temp, det, sdet, c, dim, n, i, j, iters, totalprod);
            caso = 3;
            exit();
        }
savePoint3:

        fclose(fout);
        usleep(100000);
    }

    return(0);
}
