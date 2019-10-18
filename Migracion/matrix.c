#include <stdio.h>
#include <signal.h> // para poder usar  o captar la recepción de la señal
#include <stdbool.h> // para poder usar booleanos en lugar de int como bvanderas
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

bool interrupcion = false;

void handler(int signum){
	if (signum == SIGINT) {
		printf("Empaquetando para migrar el programa: ");
		interrupcion = true;
	}
}

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

/*------------------*/
/*
Crea el archivo respaldo.dat,y guarda toda la información necesaria para la 
migración y el restarteo del programa
*/
void guardar(double matrizA[15][15], double matrizB[15][15],
		double temp[15][15], double* det, double * sdet, double* c,
		int* dim, int* n, int* i, int* iters, int* totalprod){

	
}

/*------------------*/
/*
Inicializa todas las variables en caso de encontrar una archivo de respaldo.dat
*/
void inicializar(FILE* respaldo, double matrizA[15][15], double matrizB[15][15],
		double temp[15][15], double* det, double * sdet, double* c,
		int* dim, int* n, int* i, int* iters, int* totalprod){

	fscanf(respaldo, "%d %lf", dim, det);
	lea(respaldo, A, *dim);
	lea(respaldo, B, *dim);
}
/* --------------------------MAIN---------------------------------*/
int main(void) {
/* ----- Variables -----*/
	FILE *fdata, *fout;
	int dim, n, i, j, iters, totalprod;
	double A[15][15], B[15][15], I[15][15], Temp[15][15], det, sdet, c;

	/* --- Enlace a señal ---*/
	signal(SIGINT,handler);


	/* --- Comprobación de respaldo ---*/
	FILE * respaldo = fopen("respaldo.bin", "r");
	if( respaldo ) {
		inicializar(respaldo, A, B, Temp, &det, &sdet, &c, &dim, &n, &i, &iters, &totalprod);
		fclose(respaldo);
	}

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
			// se debe de llamar a guardar(); y enviarle todos los datos necesarios
			// y luego hacer un exit para terminar el programa.
			FOR1:
		}
		for (i=0; i<n; i++) {
			mult(I, B, Temp, dim);
			scalar(I, dim, c);
			// se debe de llamar a guardar(); y enviarle todos los datos necesarios
			// y luego hacer un exit para terminar el programa.
			FOR2:
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
		fclose(fout);
		usleep(100000);
	}

	return(0);
}
