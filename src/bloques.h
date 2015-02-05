/*  
     Fichero: bloques.h
       Autor: Carlos Marin
       Fecha: 07/06/2014
 Descripcion: Cabecera en la que declararemos las funciones basicas del sistema de ficheros   
 */ 
 
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // Modos de apertura y funcion open()
#include <stdlib.h>
#include <string.h>

#include "semaforo_mutex_posix.h"

#define tamBloque 1024 //Bytes

//Sistema de ficheros
int bmount (const char *camino);
int bumount ();
int bwrite (unsigned int bloque, const void *buf);
int bread (unsigned int bloque, void *buf);

//Semaforos
void mi_waitSem();
void mi_signalSem();
