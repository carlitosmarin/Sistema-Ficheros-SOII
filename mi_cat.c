/*  
     Fichero: mi_cat.c
       Autor: Carlos Marin
       Fecha: 21/06/2014
 Descripcion: Programa cliente. Muestra el contenido de un fichero.
 		 Uso: ./mi_cat disco camino
 */ 
 
#include "directorios.h"

int main (int argc, char **argv) {	
	if (argc != 3) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	
	char *camino = argv[2];
    if(camino[strlen(camino)-1] == '/'){
        printf("ERROR: ( %s ) no es un fichero.\n",camino);
        return -1;
    }
    
	bmount(argv[1]);
 
 	int leidos = 0;
	unsigned char buf[tamBloque];
	memset(buf,0,tamBloque);
	int x = mi_read(camino,buf,0,tamBloque);
	unsigned int bucle;
	for(bucle = tamBloque; x > 0; bucle += tamBloque){ // Mientras no de error seguimos leyendo
		write(1,buf,x); //Mostrar los resultados por pantalla
		leidos += x;
		memset(buf,0,tamBloque);
		x = mi_read(camino,buf,bucle,tamBloque);
	}
	
	char string[128];
	sprintf(string, "Bytes Leídos %d\n", leidos); //Hay que mostrar el valor del num de bytes leidos y del tamanyo en bytes logico del inodo
	write(2, string, strlen(string)); //No sumara lo que ocupe esa informacion al fichero externo
	
	bumount();
}
