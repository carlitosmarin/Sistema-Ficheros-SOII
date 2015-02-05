/*
     Fichero: mi_ls.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Programa cliente. Lista el contenido de un directorio
 		 Uso: ./mi_ls disco /directorio
 */

#include "./src/directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	//Montamos el disco
	if(bmount(argv[1]) < 0){
		printf("No se ha podido abrir el fichero: %s", argv[1]);
		return 0;
	}

	char buffer[64*200];
	memset(buffer,0,tamBloque);
	printf("Directorio %s:\n", argv[2]);

	if(mi_dir (argv[2],(char *)buffer) >= 0) {
		printf("%c[%d;%dmTipo\tPerm.\tmTime\t\t\tNombre%c[%dm\n",27,0,32,27,0);
		puts("-------------------------------------------------------------------");
		printf("%s\n", buffer);
	} else puts("Error: No se ha podido llevar a cabo mi_ls\n");

	bumount();
	return 0;
}
