/*  
     Fichero: bloques.c
       Autor: Carlos Marin
       Fecha: 07/06/2014
 Descripcion: Las lecturas y escrituras sobre el sistema de ficheros se hacen bloque a bloque
 */ 
 
#include "bloques.h"

static sem_t *mutex; //Variable global del semaforo
static int fichero = 0; //El descriptor del fichero se almacena como variable estatica global

int bmount (const char *camino) { 
	fichero = open(camino, O_RDWR|O_CREAT, 0666); 
	if(fichero < 0) printf("Error en la apertura del dispostivo\n");
	mutex = initSem();
	return fichero;
}

int bumount () { 
	if(close(fichero) < 0) printf("Error en la clausura del dispostivo\n");
	deleteSem();
	return fichero;
}

int bwrite (unsigned int bloque, const void *buf) {
	if(lseek(fichero,bloque*tamBloque, SEEK_SET) == -1) //Error al ubicar el fichero
		return -1;
	return write(fichero, buf, tamBloque);
}

int bread (unsigned int bloque, void *buf) { 
	if(lseek(fichero,bloque*tamBloque, SEEK_SET) == -1) //Error al ubicar el fichero
		return -1;
	return read(fichero, buf, tamBloque);
}


/*
	Definiremos unas funciones propias para llamar a waitSem y signalSem (de esta manera todas las llamadas a las funciones de semaforo_mutex_posix.c estarán concentradas en bloques.c, y si cambiásemos el semáforo no habría que tocar el código del resto de programas)
*/
void mi_waitSem() {
	waitSem(mutex);
}

void mi_signalSem() {
	signalSem(mutex);
}
