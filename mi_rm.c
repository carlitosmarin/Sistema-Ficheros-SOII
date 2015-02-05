/*
     Fichero: mi_rm.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Programa cliente. Borra un fichero o directorio.
 		 Uso: ./mi_rm disco /ruta
 */

#include "./src/directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	//Montamos el disco
	bmount(argv[1]);

	struct STAT stat;
	if(mi_stat(argv[2],&stat) < 0){
		bumount();
		return 0;
	}

	if(stat.tipo == 'd' && stat.tamBytesLogicos != 0){
		printf("El directorio %s no está vacio, no puede ser borrado\n", argv[2]);
		bumount();
		return 0;
	}
	if(mi_unlink(argv[2]) == 0){
		puts("Se ha borrado correctamente.");
	} else ("Error: No se ha podido eliminar el fichero con mi_rm\n");

	bumount();
	return 0;
}
