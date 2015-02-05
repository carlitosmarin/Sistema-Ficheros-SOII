/*  
     Fichero: ficheros_basico.c
       Autor: Carlos Marin
       Fecha: 07/06/2014
 Descripcion: Funciones de tratamiento basico del sistema de ficheros   
 */

#include "ficheros_basico.h"

int tamMB (unsigned int bloques) { 
	if((bloques/8)%tamBloque == 0) {
		return (bloques/8)/tamBloque;
	} else {
		return ((bloques/8)/tamBloque)+1;
	}
}

int tamAI (unsigned int inodos) { 
	if((inodos*tamInodo)%tamBloque == 0) {
		return (inodos*tamInodo)/tamBloque;
	} else {
		return ((inodos*tamInodo)/tamBloque)+1;
	}
}

/*
	Tratamiento de errores: 
		-1 := Error BWRITE
 */
int initSB (unsigned int bloques, unsigned int inodos) { 
	struct superbloque sb; //Definimos la zona de memoria (variable de tipo superbloque)
	sb.posPrimerBloqueMB = posSB+1;
	sb.posUltimoBloqueMB = sb.posPrimerBloqueMB+tamMB(bloques)-1;
	sb.posPrimerBloqueAI = sb.posUltimoBloqueMB+1;
	sb.posUltimoBloqueAI = sb.posPrimerBloqueAI+tamAI(inodos)-1;
	sb.posPrimerBloqueDatos = sb.posUltimoBloqueAI+1;
	sb.posUltimoBloqueDatos = bloques-1;
	sb.posInodoRaiz = 0;
	sb.posPrimerInodoLibre = 0;
	sb.cantBloquesLibres = bloques;
	sb.cantInodosLibres = inodos;
	sb.totBloques = bloques;
	sb.totInodos = inodos;
	
	if(bwrite(posSB,&sb) < 0) return -1; //Error BWRITE	
	return 0;
}

/*
	Tratamiento de errores: 
		-1 := Error BREAD
		-2 := Error BWRITE
		-3 := Error ESCRIBIR_BIT
 */
int initMB (unsigned int bloques) { 
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1; //Leemos el superbloque para localizar el mapa de bits Error BREAD
	 
	unsigned char buf[tamBloque];
	memset(buf,0,tamBloque); //Definimos el array del tamanyo de un bloque con todos los bits a cero
	
	unsigned int bucle;
	for (bucle = sb.posPrimerBloqueMB; bucle <= sb.posUltimoBloqueMB; bucle++){
		if(bwrite(bucle,buf) < 0) return -2; //Error BWRITE
	}
	
	for(bucle = posSB; bucle <= sb.posUltimoBloqueAI; bucle++){
		if(escribir_bit(bucle,1) < 0) return -3; //Error ESCRIBIR_BIT
		sb.cantBloquesLibres--; //Restaremos los bloques (SB, MB, AI) de la cantidad de bloques libres
	} 
	
	if(bwrite(posSB,&sb) < 0) return -2; //Error BWRITE
	
	return 0;
}

/*
	Tratamiento de errores: 
		-1 := Error BREAD
		-2 := Error BWRITE
 */
int initAI (unsigned int inodos) { 
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1; //Leemos el superbloque para localizar el mapa de bits Error BREAD
	
	struct inodo in[tamBloque/tamInodo];
	unsigned int x, i, j;
	x = 1;
	for(i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++) {
		for(j = 0; j < tamBloque/tamInodo; j++) {
			in[j].tipo = 'l';
			if(x < inodos) {
				in[j].punterosDirectos[0] = x;
				x++;
			} else {
				in[j].punterosDirectos[0] = UINT_MAX;
				j = tamBloque/tamInodo;
			}
		}
		if(bwrite(i,&in) < 0) return -2; //Escribimos el array de inodos Error BWRITE
	}
	return 0;
} 

/*
	Tratamiento de errores: 
		-1 := Error BREAD
 */
int leerSB (){
	struct superbloque sb;
	if(bread(posSB,&sb) < 0) return -1; //Leemos el superbloque para localizar el mapa de bits Error BREAD
	printf("--------Información SUPERBLOQUE---------\n");
	printf("Primer Bloque MB: %d\n", sb.posPrimerBloqueMB);
	printf("Ultimo Bloque MB: %d\n", sb.posUltimoBloqueMB);
	printf("Primer Bloque AI: %d\n", sb.posPrimerBloqueAI);
	printf("Ultimo Bloque AI: %d\n", sb.posUltimoBloqueAI);
	printf("Primer Bloque Datos: %d\n", sb.posPrimerBloqueDatos);
	printf("Ultimo Bloque Datos: %d\n", sb.posUltimoBloqueDatos);
	printf("Posición Inodo Raiz: %d\n", sb.posInodoRaiz);
	printf("Primer Inodo Libre: %d\n", sb.posPrimerInodoLibre);
	printf("Cantidad Bloques Libres: %d\n", sb.cantBloquesLibres);
	printf("Cantidad Inodos Libres: %d\n", sb.cantInodosLibres);
	printf("Cantidad Bloques Totales: %d\n", sb.totBloques);
	printf("Cantidad Inodos Totales: %d\n", sb.totInodos);
	return 0;
}

/*
	Tratamiento de errores: 
		-1 := Error BREADS
		-2 := Error BREADMB
		-3 := Error BWRITE
 */
int escribir_bit (unsigned int bloque, unsigned int bit) { 
	struct superbloque sb; 
	if(bread(posSB,&sb) < 0) return -1; //Leemos el superbloque para localizar el mapa de bits Error BREADS
	
	int posbyte = bloque/8, posbit = bloque%8;
	int MB = (posbyte/tamBloque) + sb.posPrimerBloqueMB; //Bloque cuyo bit actualizaremos
	posbyte %= tamBloque; //Para que se ajuste a un bloque
		
	unsigned char mapaBits[tamBloque];
	if(bread(MB,mapaBits) < 0) return -2; // Error BREADMB
	
	unsigned char mascara = 128; //10000000
	mascara >>= posbit; //Desplazamiento de bits a la derecha
	if(bit == 0) {
		mapaBits[posbyte]&=~mascara; //AND y NOT para bits
	} else { //bit == 1
		mapaBits[posbyte]|=mascara; //OR para bits
	}
	if(bwrite(MB,mapaBits) < 0) return -3; //Error BWRITE;
	return 0;		
}

/*
	Tratamiento de errores: 
		-1 := Error BREADS
		-2 := Error BREADMB
 */ 
unsigned char leer_bit (unsigned int bloque) {
	struct superbloque sb; 
	if(bread(posSB,&sb) < 0) return -1; //Leemos el superbloque para localizar el mapa de bits Error BREADS
	
	int posbyte = bloque/8, posbit = bloque%8;
	int MB = (posbyte/tamBloque) + sb.posPrimerBloqueMB; //Bloque cuyo bit actualizaremos
	posbyte %= tamBloque; //Para que se ajuste a un bloque
		
	unsigned char mapaBits[tamBloque];
	if(bread(MB,mapaBits) < 0) return -2; // Error BREADMB
	
	unsigned char mascara = 128; //10000000
	mascara >>= posbit; //Desplazamiento de bits a la derecha
	mascara &= mapaBits[posbyte]; //Operador AND para bits
	mascara >>= (7-posbit); //Desplazamiento de bits a la derecha
	return mascara; //0 o 1
} 

/*
	Tratamiento de errores: 
		-1 := Error BREADS
		-2 := Error NO_BLOQUES_LIBRES
		-3 := Error BREADMB
		-4 := Error ESCRIBIR_BIT
		-5 := Error BWRITE
 */
int reservar_bloque () { 
	struct superbloque sb; 
	if(bread(posSB,&sb) < 0) return -1; //Error BREADS
	if(sb.cantBloquesLibres <= 0) return -2; //Error NO_BLOQUES_LIBRES
	unsigned char bufferAux[tamBloque];
	memset(bufferAux,255,tamBloque);
	unsigned char mapaBits[tamBloque];
	int nMB = sb.posPrimerBloqueMB;
	
	if(bread(nMB, mapaBits) < 0) return -3; //Error BREADMB
	
	while(memcmp(bufferAux,mapaBits,tamBloque) == 0){   					
		nMB++;
		if(bread(nMB, mapaBits) < 0) return -3; //Error BREADMB    
	}
	
	unsigned char byte = 255;
	unsigned char mascara = 128;
	int posbyte = 0, posbit = 0, fin = 0;
	while(posbyte < tamBloque && !fin){ 
		byte = mapaBits[posbyte];
		if (byte < 255) { // hay bits a 0 en el byte
			while (byte & mascara) { // operador AND para bits
				byte <<= 1;	// desplazamiento de bits a la izquierda
				posbit++;
			}
            fin = 1;
		} else posbyte++; // si el byte no contiene 0s vamos al siguiente
	}
	int numbloque = ((nMB-sb.posPrimerBloqueMB)*tamBloque+posbyte)*8+posbit;
	if(escribir_bit(numbloque, 1) < 0) return -4; //Error ESCRIBIR_BIT
	sb.cantBloquesLibres--;
	if (bwrite(posSB, &sb) < 0) return -5; //Error BWRITE
		
	return numbloque; 
}
 
/*
	Tratamiento de errores: 
		-1 := Error BREAD
		-2 := Error ESCRIBIR_BIT
		-3 := Error BWRITE
 */
int liberar_bloque (unsigned int bloque) { 
	struct superbloque sb; 
	if(bread(posSB,&sb) < 0) return -1; //Error BREAD
	if(escribir_bit(bloque,0) < 0) return -2; //Error ESCRIBIR_BIT
	sb.cantBloquesLibres++;
	if(bwrite(posSB,&sb) < 0) return -3; //Error BWRITE
	return bloque;
}

/*
	Tratamiento de errores: 
		-1 := Error BREAD
		-2 := Error BWRITE
 */
int escribir_inodo(struct inodo inodo, unsigned int ninodo) {
	struct superbloque sb; 
	if(bread(posSB,&sb) < 0) return -1; //Error BREAD
	int posIN = ((ninodo*tamInodo)/tamBloque)+sb.posPrimerBloqueAI;
	struct inodo inodos[tamBloque/tamInodo];
	if(bread(posIN,&inodos) < 0) return -1; //Leemos el array de inodos Error BREAD
	int final = ninodo%(tamBloque/tamInodo);
	inodos[final] = inodo; 
	if(bwrite(posIN,&inodos) < 0) return -2; //Error BWRITE
	return 0;
}

/*
	Tratamiento de errores: 
 */
struct inodo leer_inodo(unsigned int inodo) {
	struct superbloque sb; 
	bread(posSB,&sb);
	int posIN = ((inodo*tamInodo)/tamBloque)+sb.posPrimerBloqueAI;
	struct inodo inodos[tamBloque/tamInodo];
	bread(posIN,&inodos); //Leemos el array de inodos
	return inodos[inodo%(tamBloque/tamInodo)];
}

/*
	Tratamiento de errores: 
		-1 := Error BREAD
		-2 := Error NO_INODOS_LIBRES
		-3 := Error BWRITE
		-4 := Error ESCRIBIR_INODO
 */
int reservar_inodo (unsigned char tipo, unsigned char permisos) {
	struct superbloque sb; 
	if(bread(posSB,&sb) < 0) {
		puts("Error a la hora de leer el superbloque");
		return -1; //Error BREAD
	} 
	if(sb.cantInodosLibres == 0) {
		puts("No hay inodos libres");
		return -2; //Error NO_INODOS_LIBRES
	} 
	int posPIL = sb.posPrimerInodoLibre;
	
	struct inodo inodoR = leer_inodo(posPIL);
	
	//Modificamos el SuperBloque
	sb.posPrimerInodoLibre = inodoR.punterosDirectos[0];
	sb.cantInodosLibres--; //Al reservar un inodo, un inodo libre menos
	if(bwrite(posSB,&sb) < 0) {
		puts("Error a la hora de escribir en el superbloque");
		return -3; //Error BWRITE	
	}
	
	//Inicializamos el inodo
	inodoR.tipo = tipo;
	inodoR.permisos = permisos;
	inodoR.atime = time(NULL);
	inodoR.mtime = time(NULL);
	inodoR.ctime = time(NULL);
	inodoR.cantLinks = 1;
	inodoR.tamBytesLogicos = 0;
	inodoR.cantBloquesOcupados = 0;
	
	//Inicializamos los punteros
	int bucle;
	for(bucle = 0; bucle < puntD; bucle++) inodoR.punterosDirectos[bucle] = 0;
	for(bucle = 0; bucle < puntI; bucle++) inodoR.punterosIndirectos[bucle] = 0;
	if(escribir_inodo(inodoR,posPIL) < 0) {
		puts("Error a la hora de escribir el inodo");	
		return -4; //Escribimos el nuevo inodo Error ESCRIBIR_INODO
	}
	return posPIL; //Devolvemos le numero de inodo reservado
}

/*
	Tratamiento de errores: 
		-1 := Error BREAD
		-2 := Error LBI
		-4 := Error BWRITE
		-5 := Error ESCRIBIR_INODO
 */
int liberar_inodo (unsigned int inodo) {
	if(liberar_bloques_inodo(inodo,0) < 0) return -2;	//Llamar a la funcion auxiliar para liberar los bloques Error LBI
	struct inodo in;
	struct superbloque sb;
	
	if(bread(posSB,&sb) < 0) return -1; //Leer el superbloque Error BREAD
	in = leer_inodo(inodo);
	in.tipo = 'l'; //Marcar el inodo como tipo libre
	in.punterosDirectos[0] = sb.posPrimerInodoLibre; //Incluirlo en la lista de inodos libres
	sb.posPrimerInodoLibre = inodo; //Actualizar el superbloque
	sb.cantInodosLibres++; //Incrementar la cantidad de inodos libres
	if(bwrite(posSB,&sb) < 0) return -4; //Escribir el superbloque Error BWRITE
	if(escribir_inodo(in, inodo) < 0) return -5; //Escribir el inodo Error ESCRIBIR_INODO
	return inodo; //Devolver el num del inodo liberado	
}

/*
	La funcion liberar_bloques_inodo libera todos los blqoues (con la ayuda de la funcion liberar_bloque) a partir del bloque logico indicado por el argumento blogico. 
	Tratamiento de errores: 
		-1 := Error BREAD
		-4 := Error BWRITE
		-5 := Error ESCRIBIR_INODO
 */
int liberar_bloques_inodo (unsigned int inodo, unsigned int blogico) {	
	struct inodo in;
	in = leer_inodo(inodo);
	
	//Tipos de punteros
	unsigned int npdirectos = puntD; // Directos = 12
	unsigned int npindirectos0 = tamBloque/sizeof(unsigned int); //en 1024 bytes caben 256 punteros
	unsigned int npindirectos1 = npindirectos0*npindirectos0;
	unsigned int npindirectos2 = npindirectos1*npindirectos0;
	
	int ultBloque = in.tamBytesLogicos/tamBloque, blog = blogico, bAuxiliar; //Ultimo bloque con contenido
	if(ultBloque < blogico) return 0;
	int bucle, bucle2, bucle3;
	int punt[3] = {0,0,0}; //Indica el nivel de cada puntero
	
    unsigned int bufferNivel0[npindirectos0];
    unsigned int bufferNivel1[npindirectos0];
    unsigned int bufferNivel2[npindirectos0];
    unsigned char bufferAuxiliar[tamBloque];
    memset(bufferAuxiliar,0,tamBloque);
    
	int idx;
	
	if(blog < npdirectos) idx = -1; //Si esta en los punteros directos
	else if (blog < npdirectos+npindirectos0) { //Indirectos 0
		idx = 0;
		punt[idx] = blog - npdirectos;
	} else if (blog < npdirectos+npindirectos0+npindirectos1) {
		idx = 1;
		punt[idx-1] = (blog-(npdirectos+npindirectos0))%npindirectos0; //puntero nivel 0
		punt[idx] = (blog-(npdirectos+npindirectos0))/npindirectos0; //puntero nivel 1
	} else if (blog < npdirectos+npindirectos0+npindirectos1+npindirectos2) {
		idx = 2;
		punt[idx-2] = ((blog-(npdirectos+npindirectos0+npindirectos1))%npindirectos1)%npindirectos0; //puntero nivel 0
		punt[idx-1] = ((blog-(npdirectos+npindirectos0+npindirectos1))%npindirectos1)/npindirectos0; //puntero nivel 1
		punt[idx] = (blog-(npdirectos+npindirectos0+npindirectos1))/npindirectos1; //puntero nivel 2
	}
	
	if (idx == -1) {
		for(; blog <= ultBloque && blog < npdirectos; blog++) {
		//PUNTEROS DIRECTOS
		//El bloque logico es uno de los 12 primeros bloques logicos del inodo
			if(in.punterosDirectos[blog] > 0) {
				liberar_bloque(in.punterosDirectos[blog]);
				in.cantBloquesOcupados--;
				in.ctime = time(NULL);
			}
			in.punterosDirectos[blog] = 0; //0 el puntero actual
		}
		idx = 0; //Pasamos a punteros indirectos0
	} 
		//PUNTEROS INDIRECTOS 0
		//El bloque logico esta comprendido entre 12 y el 267 (12+256-1)
	if(idx = 0 && in.punterosIndirectos[idx] != 0) { //Si apunta a algo
		if(bread(in.punterosIndirectos[idx],bufferNivel0) < 0) return -1; //Leer el bloque de punteros de nivel 0
		for(bucle = punt[idx]; blog <= ultBloque && bucle < npindirectos0; bucle++, blog++) {
			if(bufferNivel0[bucle] != 0) {
				liberar_bloque(bufferNivel0[bucle]);
				in.cantBloquesOcupados--;
				in.ctime = time(NULL);
				bufferNivel0[bucle] = 0;
			}
		}
		if(strcmp((const char *)bufferNivel0,(const char *)bufferAuxiliar) == 0) { 
			//No quedan punteros ocupados en el bloque de nivel 0
			liberar_bloque(in.punterosIndirectos[idx]);
			in.punterosIndirectos[0] = 0;
			in.cantBloquesOcupados--;
			in.ctime = time(NULL);
		} else {
			if(bwrite(in.punterosIndirectos[idx],bufferNivel0) < 0) return -2; //Error BWRITE 
			//Escribimos el bloque modificado				
		}
		idx = 1; //Pasamos a punteros indirectos 1
	} else if(idx == 0) {
		blog = npindirectos0 + npdirectos;
		idx = 1;
	}
	//PUNTEROS INDIRECTOS 1
	//El bloque logico esta comprendido entre 268 y el 65.803 (12+256+256²-1)
	if(idx == 1 && in.punterosIndirectos[idx] != 0) {
		if(bread(in.punterosIndirectos[idx],bufferNivel1) < 0) return -1; //Leer el bloque de punteros de nivel 1
		for(bucle = punt[idx]; blog <= ultBloque && bucle < npindirectos0; bucle++){
		bAuxiliar = blog;
		if(bufferNivel1[bucle] != 0) {
			if(bread(bufferNivel1[bucle], bufferNivel0) < 0) return -1;
			for(bucle2 = punt[idx-1]; bAuxiliar <= ultBloque && bucle2 < npindirectos0; bucle2++, bAuxiliar++) {
				if(bufferNivel0[bucle2] != 0) {
					liberar_bloque(bufferNivel0[bucle2]);
					in.cantBloquesOcupados--;
					in.ctime = time(NULL);
					bufferNivel0[bucle2] = 0;
				}	
			}
			if(strcmp((const char *)bufferNivel0,(const char *)bufferAuxiliar) == 0) { 
				//No quedan punteros ocupados en el bloque de nivel 0
				liberar_bloque(bufferNivel1[bucle]);
				bufferNivel1[bucle] = 0;
				in.cantBloquesOcupados--;
				in.ctime = time(NULL);
			} else {
			if(bwrite(bufferNivel1[bucle],bufferNivel0) < 0) return -2; //Error BWRITE 
			//Escribimos el bloque modificado
			}
		} else blog += npindirectos0;
		
		punt[idx-1] = 0;
			
		}
		//Comprobamos si tenemos que liberar los punteros
		if(strcmp((const char *)bufferNivel1,(const char *)bufferAuxiliar) == 0) { 
			liberar_bloque(in.punterosIndirectos[idx]); 
			in.punterosIndirectos[idx] = 0;
			in.cantBloquesOcupados--;
			in.ctime = time(NULL);	
		} else {
			if(bwrite(in.punterosIndirectos[idx],bufferNivel1) < 0) return -2;
		}
		idx = 2; //Pasamos a punteros indirectos 2
	} else if (idx == 1) {
		blog = npdirectos+npindirectos0+npindirectos1;
		idx = 2;
	}
	
	//PUNTEROS INDIRECTOS 2
	//El bloque logico esta comprendido entre 65.804 y el 16.843.019 (12+256+256²+256³-1)
	if(idx == 2 && in.punterosIndirectos[idx] != 0) { 
		if(bread(in.punterosIndirectos[idx],bufferNivel2) < 0) return -1; //Leer el bloque de nivel 2
		for(bucle3 = punt[idx]; blog <= ultBloque && bucle3 < npindirectos0; bucle3++) {
			if(bufferNivel2[bucle3] != 0) {
				if(bread(bufferNivel2[bucle3],bufferNivel1) < 0) return -1;
				for(bucle2 = punt[idx-1]; blog <= ultBloque && bucle2 < npindirectos0; bucle2++) {
					bAuxiliar = blog;
					if(bufferNivel1[bucle2] != 0) {
						if(bread(bufferNivel1[bucle2], bufferNivel0) < 0) return -1;
						for(bucle = punt[idx-2]; bAuxiliar <= ultBloque && bucle < npindirectos0; bucle++, bAuxiliar++) {
							if(bufferNivel0[bucle] != 0) {
								liberar_bloque(bufferNivel0[bucle]);
								in.cantBloquesOcupados--;
								in.ctime = time(NULL);
								bufferNivel0[bucle] = 0;
							}	
						}
						//Comprobar si tenemos  que liberar punteros
						if(strcmp((const char *)bufferNivel0,(const char *)bufferAuxiliar) == 0) { 
							//No quedan punteros ocupados en el bloque de nivel 0
							liberar_bloque(bufferNivel1[bucle2]);
							bufferNivel1[bucle2] = 0;
							in.cantBloquesOcupados--;
							in.ctime = time(NULL);
						} else {
							if(bwrite(bufferNivel1[bucle2],bufferNivel0) < 0) return -2; 
						}	
					}
					blog += npindirectos0;
					punt[idx-2] = 0;
				}
				//Comprobar si tenemos  que liberar punteros
				if(strcmp((const char *)bufferNivel0,(const char *)bufferAuxiliar) == 0) { 
					//No quedan punteros ocupados en el bloque de nivel 0
					liberar_bloque(bufferNivel2[bucle3]);
					bufferNivel2[bucle3] = 0;
					in.cantBloquesOcupados--;
					in.ctime = time(NULL);
				} else {
					if(bwrite(bufferNivel2[bucle3],bufferNivel1) < 0) return -2; 
				}	
			}
			punt[idx-1] = 0;
		}
		//Comprobar si tenemos  que liberar punteros
		if(strcmp((const char *)bufferNivel2,(const char *)bufferAuxiliar) == 0) { 
			//No quedan punteros ocupados en el bloque de nivel 0
			liberar_bloque(in.punterosIndirectos[idx]);
			in.punterosIndirectos[idx] = 0;
			in.cantBloquesOcupados--;
			in.ctime = time(NULL);
		} else {
			if(bwrite(in.punterosIndirectos[idx-1],bufferNivel1) < 0) return -2; 
		}
	}		
		
	if(escribir_inodo(in, inodo) < 0) return -3; //Escribimos el inodo modificado
	return 0;
}

int traducir_bloque_inodo (unsigned int inodo, unsigned int blogico, unsigned int *bfisico, char reservar) {
	//Almenos dos tipos de error, ERROR solo consulta (-2)
	struct inodo in;
	in = leer_inodo(inodo);
	
	//Tipos de punteros
	int npdirectos = puntD; // Directos = 12
	int npindirectos0 = tamBloque/sizeof(unsigned int); //en 1024 bytes caben 256 punteros
	int npindirectos1 = npindirectos0*npindirectos0;
	int npindirectos2 = npindirectos1*npindirectos0;
	
	int ultBloque = in.tamBytesLogicos/tamBloque; //Ultimo bloque con contenido
	int bucle, bfis;
	int existe = 0;
	unsigned int buf[npindirectos0];
	unsigned int punt[3]; //Indica el nivel de cada puntero
	int idx;
	
	if(blogico < npdirectos) idx = -1; //Punteros directos
	else if (blogico < npdirectos+npindirectos0) { //Indirectos 0
		idx = 0;
		punt[idx] = blogico-npdirectos;
	} else if (blogico<npdirectos+npindirectos0+npindirectos1) {
		idx = 1;
		punt[idx-1] = (blogico-(npdirectos+npindirectos0))%npindirectos0; //puntero nivel 0
		punt[idx] = (blogico-(npdirectos+npindirectos0))/npindirectos0; //puntero nivel 1
	} else if (blogico < npdirectos+npindirectos0+npindirectos1+npindirectos2) {
		idx = 2;
		punt[idx-2] = ((blogico-(npdirectos+npindirectos0+npindirectos1))%npindirectos1)%npindirectos0; //puntero nivel 0
		punt[idx-1] = ((blogico-(npdirectos+npindirectos0+npindirectos1))%npindirectos1)/npindirectos0; //puntero nivel 1
		punt[idx] = (blogico-(npdirectos+npindirectos0+npindirectos1))/npindirectos1; //puntero nivel 2
	}
	
	if (idx >= 0) { //Controlamos los punteros indirectos
		if (in.punterosIndirectos[idx] != 0) { //Si apunta a un bloque
			existe = 1;
			bfis = in.punterosIndirectos[idx];
			int n; 
			for(n = idx; n > -1 && existe; n--) { //Visitamos los niveles
				if(bread(bfis, buf) < 0) return -1;
				if(buf[punt[n]] == 0) existe = 0;
				else bfis = buf[punt[n]];
			}
		} 	
	} else if(in.punterosDirectos[blogico] != 0) { //Puntero directo y apunta a un bloque
		existe = 1;
		bfis = in.punterosDirectos[blogico];
	}
	
	if (existe) { //Si existe se le asignara un bloque
		*bfisico = bfis;
		return 0;
	}
	
	unsigned char buf2[tamBloque];
	memset(buf2,0,tamBloque); //Inicializamos el bloque a 0
	int ninodo; 
	
	if(!reservar) return -2; //Si solo se quiere consultar!!
	else if (reservar){ //Si queremos reservar
		if (idx == -1) { //Se trata de un puntero Directo
			if((ninodo = reservar_bloque()) < 0) return -1; //Error RESERVAR_BLOQUE
			if(bwrite(ninodo, buf2) < 0) return -1; //Error ESCRITURA
			in.punterosDirectos[blogico] = ninodo;
			in.cantBloquesOcupados++;
			in.ctime = time(NULL);
			*bfisico = in.punterosDirectos[blogico];
		} else { //Se trata de un puntero indirecto
			if(in.punterosIndirectos[idx] == 0) { //No apunta a nada
				if((ninodo = reservar_bloque()) < 0) return -1; //Error RESERVAR_BLOQUE
				if(bwrite(ninodo, buf2) < 0) return -1; //Error ESCRITURA
				in.punterosIndirectos[idx] = ninodo;
				in.cantBloquesOcupados++;
				in.ctime = time(NULL);	
			}
			bfis = in.punterosIndirectos[idx]; //Asignamos el puntero
			for(bucle = idx; bucle > -1; bucle--) { //Recorremos todos los niveles de los indirectos
				if(bread(bfis,buf) < 0) return -1;
				if(buf[punt[bucle]] == 0) { //No apunta a nada
					if((ninodo = reservar_bloque()) < 0) return -1; //Error RESERVAR_BLOQUE
					if(bwrite(ninodo, buf2) < 0) return -1; //Error ESCRITURA
					buf[punt[bucle]] = ninodo;
					in.cantBloquesOcupados++;
					in.ctime = time(NULL);
					if(bwrite(bfis, buf) < 0) return -1;
				}
				if(bucle == 0) *bfisico = buf[punt[bucle]]; //Es para datos
				bfis = buf[punt[bucle]];
			}
		}
		if(escribir_inodo(in,inodo) < 0) return -1; //Actualizamos el inodo
		return 0;
	}
}
