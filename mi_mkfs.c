/*
     Fichero: mi_mkfs.c
       Autor: Carlos Marin
       Fecha: 07/06/2014
 Descripcion: Programa cliente. Sirve para crear el fichero con el tamanyo adecuado
         Uso: ./mi_mkfs nombrefichero cantidad_bloques
 */

#include "./src/directorios.h"

int main (int argc, char **argv) {
	if (argc != 3) {
		puts("Número de parametros incorrectos\n");
		return -1;
	}

	unsigned int cantidad_bloques = atoi(argv[2]); //argv[2] = cantidad_bloques

	if(cantidad_bloques <= 0) {
		puts("ERROR: El número de bloques debe ser mayor a 0\n");
		return -1;
	}

	//Montamos el dispositivo
	printf("Montando el dispositivo..");
	bmount (argv[1]); //argv[1] = nombre_fichero
	puts(" OK");

	unsigned char buf[tamBloque];
	memset(buf,0,tamBloque);
	printf("Reiniciando memoria del dispistivo..");
	unsigned int bucle;
	for (bucle = 0; bucle < cantidad_bloques; bucle++) bwrite (bucle, buf); //Llenamos bloques con 0
	puts(" OK");

	int inodos = cantidad_bloques/4; //Calcularemos los inodos

	printf("Iniciando el SuperBloque..");
	if(initSB(cantidad_bloques,inodos) < 0) {
		puts("Error al iniciar el SuperBloque");
		bumount (); //Desmontamos el dispositivo
		return -1;
	}
	puts(" OK");

	printf("Iniciando el Mapa de Bits..");
	if(initMB(cantidad_bloques) < 0) {
		puts("Error al iniciar el Mapa de Bits");
		bumount (); //Desmontamos el dispositivo
		return -1;
	}
	puts(" OK");

	printf("Iniciando el Array de inodos..");
	if(initAI(inodos) < 0) {
		puts("Error al iniciar el Array de inodos");
		bumount (); //Desmontamos el dispositivo
		return -1;
	}
	puts(" OK");

	printf("Reservando el Inodo Raiz..");
	reservar_inodo ('d',7);
	puts(" OK");


	//Desmontamos el dispositivo
	bumount ();

	return 0;
}
