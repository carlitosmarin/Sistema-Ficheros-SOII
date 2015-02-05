/*
     Fichero: mi_ln.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Programa cliente. Crea un enlace a un fichero.
 		 Uso: ./mi_ln disco /fichero /enlace
 */

#include "./src/directorios.h"

int main (int argc, char **argv) {
	if (argc != 4) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	if(mi_link (argv[2],argv[3]) == 0) printf("Enlace creado correctamente\n");
	else printf("Error: No se ha podido llevar a cabo mi_ln\n");

	bumount();
	return 0;
}
