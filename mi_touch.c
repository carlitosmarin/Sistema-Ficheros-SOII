/*
     Fichero: mi_touch.c
       Autor: Carlos Marin
       Fecha: 14/06/2014
 Descripcion: Programa cliente. Crea un fichero
 		 Uso: ./mi_touch disco permisos /ruta
 */

#include "./src/directorios.h"

int main (int argc, char **argv) {
	if (argc != 4) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	unsigned int permisos = atoi(argv[2]);
	if(permisos < 0 || permisos > 7) {
		puts("Permisos no válidos");
		bumount();
		return -1;
	}
	int charF = strlen(argv[3])-1;

	if(argv[3][charF]!='/'){
		if(mi_creat(argv[3], permisos) < 0){
			printf("Error: No se ha podido llevar a cabo mi_touch\n");
		} else{
			printf("Archivo creado satisfactoriamente %s\n", argv[3]);
		}
	}else { //La ruta es un directorio ya que acaba en "/"
		printf("Introduce un fichero válido para crear\n");
	}

	bumount();
	return 0;
}
