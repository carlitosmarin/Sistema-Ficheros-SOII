/*
     Fichero: ficheros.h
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Cabecera en la que declararemos las funciones de tratamiento basico de ficheros
 */

#include "ficheros_basico.h"
#include <time.h>

struct STAT { //La misma estructura INODO pero sin los punteros
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecucion)
	time_t atime; //Fecha y hora del ultimo acceso a los datos
	time_t mtime; //Fecha y hora de la ultima modificacion de los datos
	time_t ctime; //Fecha y hora de la ultima modificacion del inodo
	unsigned int cantLinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamBytesLogicos; //Tamanyo en bytes logicos
	unsigned int cantBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos

};

int mi_write_f (unsigned int inodo, const void *buf, unsigned int offset, unsigned int nbytes);
int mi_read_f (unsigned int inodo, void *buf, unsigned int offset, unsigned int nbytes);
int mi_chmod_f (unsigned int inodo, unsigned char permisos);
int mi_truncar_f (unsigned int inodo, unsigned int nbytes);
int mi_stat_f (unsigned int inodo, struct STAT *stat);