/*
     Fichero: leer_sf.c
       Autor: Carlos Marin
       Fecha: 09/06/2014
 Descripcion: Programa cliente. Muestra la metainformacion del sistema de ficheros.
 		 Uso: ./leer_sf disco
 */

#include "directorios.h"

int main (int argc, char **argv) {
	if (argc != 2) {
		printf("Numero de parametros incorrectos");
		return -1;
	}
	//Montamos el disco
	if(bmount(argv[1]) < 0){
		printf("No se ha podido abrir el fichero: %s", argv[1]);
		return -1;
	}

	int tamMB, tamAI;

	struct superbloque sb;
	if(bread(posSB,&sb) < 0) {
		printf("Error en la lectura del superbloque");
		bumount();
		return -1;
	}

	leerSB(); //Mostrar todos los campos del superbloque

	tamMB = sb.posUltimoBloqueMB - sb.posPrimerBloqueMB;
	tamAI = sb.posUltimoBloqueAI - sb.posPrimerBloqueAI;
	printf("Tamanyo MB: %d \nTamanyo AI: %d \n", tamMB, tamAI);

	printf("Tamanyo de la estructura inodo: %lu\n", sizeof(struct inodo));

	unsigned int i = 0,y = 1;

	printf("\nPulsa ENTER para mostrar la lista de inodos no libres."); getchar();
	struct tm *ts;
	char atime[80];
	char mtime[80];
	char ctime[80];
	unsigned int x = 0;
	struct inodo in;

	for(i = 0; i < sb.totInodos && x < (sb.totInodos - sb.cantInodosLibres); i++){ // Muestra todos lo ins
		in = leer_inodo(i);
		if(in.tipo != 'l'){
			x++;
			printf("----> Numero inodo: %d\n",i);
			printf("\tTipo de inodo: %c\n",in.tipo);
			printf("\tPermisos: %d\n",in.permisos);
			printf("\tNumero de links: %d\n",in.cantLinks);
			printf("\tTamanyo en bytes logicos: %d\n",in.tamBytesLogicos);
			printf("\tNumero de bloques que ocupa: %d\n",in.cantBloquesOcupados);
			ts = localtime(&in.atime);
			strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
			ts = localtime(&in.mtime);
			strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
			ts = localtime(&in.ctime);
			strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
			printf("\tATIME: %s\n\tMTIME: %s\n\tCTIME: %s\n",atime,mtime,ctime);
		}
	}

	printf("\nBLOQUES OCUPADOS (%d bloques ocupados)\n", sb.totBloques-sb.cantBloquesLibres);

	bumount();
}
