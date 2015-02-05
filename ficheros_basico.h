/*  
     Fichero: ficheros_basico.h
       Autor: Carlos Marin
       Fecha: 07/06/2014
 Descripcion: Cabecera en la que declararemos las funciones de tratamiento basico del sistema de ficheros   
 */ 
 
#include "bloques.h"
#include <time.h>
#include <limits.h>

#define posSB 0 //El superbloque se escribe en el primer bloque de nuestro sistema de ficheros   
#define tamInodo 128 //El tamanyo en bytes de un inodo
#define puntD 12 //Cantidad de punteros Directos del inodo
#define puntI 3 //Cantidad de punteros Indirectos del inodo

struct superbloque {
	unsigned int posPrimerBloqueMB; //Posicion del primer bloque del mapa de bits 
	unsigned int posUltimoBloqueMB; //Posicion del ultimo bloque del mapa de bits
	unsigned int posPrimerBloqueAI; //Posicion del priemr bloque del array de inodos
	unsigned int posUltimoBloqueAI; //Posicion del ultimo bloque del array de inodos
	unsigned int posPrimerBloqueDatos; //Posicion del primer bloque de datos
	unsigned int posUltimoBloqueDatos; //Posicion del ultimo bloque de datos
	unsigned int posInodoRaiz; //Posicion del inodo del directorio raiz
	unsigned int posPrimerInodoLibre; //Posicion del primer inodo libre
	unsigned int cantBloquesLibres; //Cantidad de bloques libres
	unsigned int cantInodosLibres; //Cantidad de inodos libres
	unsigned int totBloques; //Cantidad total de bloques
	unsigned int totInodos; //Cantidad total de inodos
	char padding [tamBloque-12*sizeof(unsigned int)]; //Relleno
};

struct inodo {
	unsigned char tipo; //Tipo (libre, directorio o fichero)
	unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecucion)
	unsigned char reservado_alineacion1[6]; //Microprocesador de 64 bits -> 6
	time_t atime; //Fecha y hora del ultimo acceso a los datos
	time_t mtime; //Fecha y hora de la ultima modificacion de los datos
	time_t ctime; //Fecha y hora de la ultima modificacion del inodo
	unsigned int cantLinks; //Cantidad de enlaces de entradas en directorio
	unsigned int tamBytesLogicos; //Tamanyo en bytes logicos
	unsigned int cantBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos
	unsigned int punterosDirectos[puntD]; //12 punteros a bloques directos
	unsigned int punterosIndirectos[puntI]; //3 punteros a bloques indirectos: Simple, Doble, Triple
	char padding [tamInodo-8*sizeof(unsigned char)-3*sizeof(time_t)-18*sizeof(unsigned int)];
};

//Funciones basicas para inicializar las diferentes estructuras
int tamMB (unsigned int bloques); //Calcula el tamanyo necesario para el mapa de bits
int tamAI (unsigned int inodos); //Calcula el tamanyo del array de inodos
int initSB (unsigned int bloques, unsigned int inodos);
int initMB (unsigned int bloques);
int initAI (unsigned int inodos); 
int leerSB (); //Metodo opcional que muestra por pantalla un superbloque

//Funciones basicas de E/S para bits del mapa de bits
int escribir_bit (unsigned int bloque, unsigned int bit); //Escribe 0 o 1 en el mapa de bits
unsigned char leer_bit (unsigned int bloque); //Lee un determinado bit del mapa de bits

//Funciones basicas para reservar y liberar inodos
int reservar_bloque (); //Encuentra el primer bloque libre, lo ocupa y devuelve su posicion
int liberar_bloque (unsigned int bloque); //Libera un bloque determinado

//Funciones basicas de E/S para inodos del array de inodos
int escribir_inodo (struct inodo inodo, unsigned int ninodo);
struct inodo leer_inodo (unsigned int inodo);

//Funciones basicas para reservar y liberar inodos
int reservar_inodo (unsigned char tipo, unsigned char permisos);
int liberar_inodo (unsigned int inodo); 
int liberar_bloques_inodo (unsigned int inodo, unsigned int blogico);
int traducir_bloque_inodo (unsigned int inodo, unsigned int blogico, unsigned int *bfisico, char reservar);



