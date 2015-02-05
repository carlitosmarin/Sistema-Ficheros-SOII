/*  
     Fichero: mi_stat.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Programa cliente. Muestra la informacion acerca del inodo de un fichero o directorio
 		 Uso: ./mi_stat disco /ruta
 */ 
 
#include "directorios.h"

int main (int argc, char **argv) {	
	if (argc != 3) {
		printf("Número de parametros incorrectos\n");
		return -1;
	}
	//Montamos el disco 
	bmount(argv[1]);
	
	struct STAT stat;
	struct tm *tm;
	
	if(mi_stat (argv[2],&stat) == 0) {
		printf("------- Informacion de la entrada %s -------\n", argv[2]);
		printf("Tipo: %c. \n", stat.tipo);
		printf("Permisos: %d. \n", stat.permisos);
		printf("Numero de links: %d. \n", stat.cantLinks);
		printf("Tamaño en Bytes Logicos: %d. \n", stat.tamBytesLogicos);
		printf("Bloques Ocupados: %d. \n", stat.cantBloquesOcupados);
		
		//Fechas (atime,mtime,ctime)
		tm = localtime(&stat.atime); //Ultimo acceso a datos
		printf("Ultimo acceso a los datos: %d-%02d-%02d %02d:%02d:%02d\t \n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
		
		tm = localtime(&stat.mtime); //Ultimo modificacion de los datos
		printf("Ultima modificacion de los datos: %d-%02d-%02d %02d:%02d:%02d\t \n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
		
		tm = localtime(&stat.ctime); //Ultimo modificacion del inodo
		printf("Ultima modificacion del inodo: %d-%02d-%02d %02d:%02d:%02d\t \n", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	} else printf("Error: No se ha podido llevar a cabo mi_stat");
	
	bumount();
	return 0;
}
