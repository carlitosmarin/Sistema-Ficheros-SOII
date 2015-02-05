/*  
     Fichero: mi_mkdir.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Programa cliente. Crea un fichero o directorio 
 		 Uso: ./mi_mkdir disco permisos /ruta
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
		puts("Permisos no válidos\n");
		bumount();
		return -1;
	}
	int charF = strlen(argv[3])-1;
	
	if(argv[3][charF] == '/') { // DIRECTORIO
		if(mi_creat(argv[3], permisos) == 0){
			printf("El directorio %s se ha creado correctamente\n", argv[3]);
		} else printf("Error: No se ha podido llevar a cabo mi_mkdir");
	} else { // FICHERO
		if(mi_creat(argv[3], permisos) == 0){
			printf("El archivo %s se ha creado correctamente\n", argv[3]);
		} else printf("Error: No se ha podido llevar a cabo mi_mkdir");
	}

	bumount();
	return 0;
}
