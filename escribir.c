/*  
     Fichero: escribir.c
       Autor: Carlos Marin
       Fecha: 09/06/2014
 Descripcion: Programa cliente. Escribe texto en uno o varios ficheros. 
 		 Uso: ./escribir disco camino offset
 */ 
 
#include "directorios.h"

int main (int argc, char **argv) {	
	if (argc != 4) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	
	char *camino = argv[2];
    if(camino[strlen(camino)-1] == '/'){
        printf("ERROR: ( %s ) no es un fichero.\n",camino);
        return -1;
    }
    
	bmount(argv[1]);
	int offset = atoi(argv[3]);

    int bytes = 0;
    char buffer[1000];
    strcpy (buffer, "blablablablablablablablablablabla...\n");
    
    int nin = 0; //= reservar_inodo ('f', 6);
    
    int n = strlen(buffer);
    int m = 70;
    int i, offsetActual;
    int er;
    
   	for(i = 0, offsetActual = offset ; i < m; i++, offsetActual += n){
   		er = mi_write(camino,&buffer,offsetActual,n);
    	if(er < 0) {
            bumount();
    		return 0;
    	}
        bytes += er;
	}

	bumount();
}
