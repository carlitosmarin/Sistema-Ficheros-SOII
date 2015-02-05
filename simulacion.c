/*
     Fichero: simulacion.c
       Autor: Carlos Marin
       Fecha: 21/06/2014
 Descripcion: Programa cliente. Escrituras simultaneas en el sistema de ficheros
 		 Uso: ./simulacion disco
 */

#include "./src/directorios.h"
#include <signal.h>
#include <sys/wait.h>
#include "./src/registro.h"

#define posMax 500000
#define PROCESOS 100 //Cantidad de procesos simultaneos

int acabados;
char directorio[100];

//El enterrador trata los procesos zombies
void reaper(){ while(wait3(NULL, WNOHANG, NULL) > 0) acabados++; }

void proceso(int pid){
	char pidDirectorio[30];
	sprintf(pidDirectorio,"proceso_%d/",pid);

	char camino[200];
	strcpy(camino,directorio);
	strcat(camino, pidDirectorio);

	if(mi_creat(camino, 7) != 0){ //Creamos el directorio del proceso
		printf("ERROR: El directorio (%s) no se ha podido crear \n", camino);
		exit(1);
	}

	char trayecto[200];
	strcpy(trayecto, camino);
	sprintf(camino, "%sprueba.dat", camino); //El nombre del fichero
	if(mi_creat(camino, 7) != 0){ //Creamos pureba.dat
		printf("ERROR: El fichero (%s) no se ha podido crear \n", camino);
		exit(1);
	}

	char buffer[64*200];
	memset(buffer,0,tamBloque);

	printf("Directorio %s:\n", camino);
	if(mi_dir (trayecto,(char *)buffer) >= 0) {
		printf("%c[%d;%dmTipo\tPerm.\tmTime\t\t\tNombre%c[%dm\n",27,0,32,27,0);
		puts("-------------------------------------------------------------------");
		printf("%s\n", buffer);
	} else puts("Error: No se ha podido llevar a cabo mi_ls\n");

	struct registro registro;
	registro.nEscritura = 0;
	srand(pid);
	//exit(0); //Antes de escriibr, solo los creamos
	int i;
	for(i = 0; i < 50; i++){//Escribimos los 50 registros
	 	registro.fecha=time(NULL); registro.pid=getpid(); registro.nEscritura=i+1; registro.posicion=rand()%posMax;
		srand(registro.posicion); //Actualizamos la semilla
		if(mi_write(camino,&registro,registro.posicion*sizeof(struct registro),sizeof(struct registro)) < 0){
			printf("ERROR: Al escribir en %s\n", camino);
			exit(1);
		} //else printf("El fichero (%s) se ha escrito bien\n", camino);
		//usleep(50000); //Esperamos 0,05 segundos
	}
	exit(0);
}

int main (int argc, char **argv) {
	if (argc != 2) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	bmount(argv[1]);

	char fech[16];
	char f[8] = "/simul_";

	struct tm *ts;
	time_t t = time(NULL);
	ts = localtime(&t);
	strftime(fech, sizeof(fech), "%Y%m%d%H%M%S/", ts);
	strcpy(directorio, f);
	strcat(directorio, fech);

	if(mi_creat(directorio, 7) < 0){ //Creamos el directorio
		printf("ERROR: El directorio (%s) no se ha podido crear\n", directorio);
		bumount(); //Desmontamos el sistema de ficheros
		return -1;
	}

	acabados = 0;
	signal(SIGCHLD, reaper);
	while(acabados < PROCESOS) {
		if(fork() == 0) proceso(getpid()); //Creamos un nuevo proceso
		usleep(200000); //Provocamos la espera de creacion de procesos (0.2 s)
	}

	if(acabados < PROCESOS) pause(); //Esperamos a que todos los procesos acaben

	char buffer[64*200];
	memset(buffer,0,tamBloque);
	printf("Directorio raiz %s:\n", directorio);

	if(mi_dir (directorio,(char *)buffer) >= 0) {
		printf("%c[%d;%dmTipo\tPerm.\tmTime\t\t\tNombre%c[%dm\n",27,0,32,27,0);
		puts("-------------------------------------------------------------------");
		printf("%s\n", buffer);
	} else puts("Error: No se ha podido llevar a cabo mi_ls\n");

	puts("\n-----------------FINIQUITAO------------------");
	bumount(); //Desmontamos el sistema de ficheros
}
