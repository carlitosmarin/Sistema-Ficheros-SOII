/*  
     Fichero: directorios.h
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Cabecera en la que declararemos las funciones de tratamiento basico de las direcciones del sistema de ficheros   
 */ 
 
#include <string.h>
#include "ficheros.h"
 
#define longEntrada 60 //Longitud de cada entrada

struct entrada {
	char nombre[longEntrada]; //En el sistema de ficheros ext2 la longitud del nombre es 255
	unsigned int inodo;
};


int extraer_camino (const char *camino, char *inicial, char *final);
int buscar_entrada (const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos);
int mi_creat (const char *camino, unsigned char permisos);
int mi_dir (const char *camino, char *buf);
int mi_link (const char *camino1, const char *camino2);
int mi_unlink (const char *camino);
int mi_chmod (const char *camino, unsigned char permisos);
int mi_stat (const char *camino, struct STAT *p_stat);
int mi_read (const char *camino, void *buf, unsigned int offset, unsigned int nbytes);
int mi_write (const char *camino, const void *buf, unsigned int offset, unsigned int nbytes);
