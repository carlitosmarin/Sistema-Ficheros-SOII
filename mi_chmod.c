/*  
     Fichero: mi_chmod.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Programa cliente. Cambia los permisos de un fichero o directorio 
 		 Uso: ./mi_chmod disco permisos /ruta
 */ 
 
#include "directorios.h"

int main (int argc, char **argv) {	
	if (argc != 4) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	//Montamos el disco 
	bmount(argv[1]);
	
	unsigned int permisos = atoi(argv[2]);
	if(permisos < 0 || permisos > 7) {
		puts("Permisos introducidos invalidos\n");
		bumount();
		return -1;
	}
	
	if(mi_chmod (argv[3],permisos) == 0) printf("Permisos del fichero %s actualizados correctamente\n", argv[3]);
	else printf("Error: No se ha podido llevar a cabo mi_chmod\n");
	
	bumount();
	return 0;
}
