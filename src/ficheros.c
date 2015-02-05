/*
     Fichero: ficheros.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Funciones de tratamiento basico de ficheros
 */

#include "ficheros.h"

/*
	Tratamiento de errores:
		-1 := Error NO_PERMISOS_ESCRITURA
		-2 := Error TBI (Traducir Bloque Inodo)
		-3 := Error BREAD
		-4 := Error BWRITE
		-5 := Error ESCRIBIR_INODO
 */
int mi_write_f (unsigned int inodo, const void *buf, unsigned int offset, unsigned int nbytes) {
	struct inodo in = leer_inodo(inodo);
	if((in.permisos & 2) != 2) return -1; //Error NO_PERMISOS_ESCRITURA

	int bloqueI = offset/tamBloque, bloqueF = (offset+nbytes-1)/tamBloque, bfisico, bytes = 0;
	unsigned char bufBloque[tamBloque];
	int desp1 = offset%tamBloque; //Desplazamiento en el bloque
	int desp2 = (offset+nbytes-1) % tamBloque;

	if(bloqueI == bloqueF) { //Si son el mismo el bloque inicial y el final
		if(traducir_bloque_inodo(inodo,bloqueI,&bfisico,1) < 0) return -2; //Obtenemos el bloque físico Error TBI
		if(bread(bfisico, bufBloque) < 0) return -3; //Leemos el bloque correspondiente Error BREAD
		memcpy (bufBloque+desp1, buf, desp2-desp1+1);
		if(bwrite(bfisico, bufBloque) < 0) return -4; // Escribimos el bloque en memoria Error BWRITE
		bytes += desp2-desp1+1;	// Aumentamos el contador los bytes que hemos escrito
	} else {
	//3 fases
	//Primer bloque
	if(traducir_bloque_inodo(inodo,bloqueI,&bfisico,1) < 0) return -2; //Error TBI
	if(bread(bfisico, bufBloque) < 0) return -3; //Leemos el bloque correspondiente Error BREAD
	memcpy (bufBloque+desp1, buf, tamBloque-desp1);
	if(bwrite(bfisico, bufBloque) < 0) return -4; // Escribimos el bloque en memoria Error BWRITE
	bytes += tamBloque-desp1;

	//Intermedios
	int bucle;
	for(bucle = bloqueI+1; bucle < bloqueF; bucle++) {
		memcpy(bufBloque, buf+(tamBloque-desp1)+(bucle-bloqueI-1)*tamBloque, tamBloque);
		if(traducir_bloque_inodo(inodo,bucle,&bfisico,1) < 0) return -2; //Error TBI
		bytes += bwrite(bfisico,bufBloque);
	}

	//Ultimo bloque
	if(traducir_bloque_inodo(inodo,bloqueF,&bfisico,1) < 0) return -2; //Error TBI
	if(bread(bfisico, bufBloque) < 0) return -3; //Leemos el bloque correspondiente Error BREAD
	memcpy(bufBloque,buf+(nbytes-desp2-1),desp2+1);
	if(bwrite(bfisico, bufBloque) < 0) return -4; // Escribimos el bloque en memoria Error BWRITE
	bytes += desp2+1;
	}
	//Actualizaremos la metainformacion del inodo
	in = leer_inodo(inodo);
	if(in.tamBytesLogicos<offset+bytes && bytes != 0) in.tamBytesLogicos = offset+bytes;
	in.mtime = time(NULL);
	in.ctime = time(NULL);
	if(escribir_inodo(in, inodo) < 0) return -5; //Escribimos el inodo modificado Error ESCRIBIR_INODO
	return bytes;
}

/*
	Tratamiento de errores:
		-2 := Error NO_PERMISOS_LECTURA
		-3 := Error TBI (Traducir Bloque Inodo)
		-4 := Error BREAD
		-5 := Error BWRITE
		-6 := Error ESCRIBIR_INODO
 */
int mi_read_f (unsigned int inodo, void *buf, unsigned int offset, unsigned int nbytes) {
	struct inodo in;
	in = leer_inodo(inodo);
	if((in.permisos & 4) != 4) return -1; //Error NO_PERMISOS_LECTURA

	// La función no puede leer más allá del tamaño en bytes lógicos del inodo
	if (offset + nbytes >= in.tamBytesLogicos)
		nbytes = in.tamBytesLogicos - offset; //Leemos sólo los bytes que podemos desde el offset hasta el final de fichero
	if (offset > in.tamBytesLogicos || in.tamBytesLogicos == 0){
		return 0; // No podemos leer nada
	}
	if(nbytes == 0) return 0; //No leemos nada

	unsigned char bufBloque[tamBloque]; //Leer el disco
	memset(bufBloque,0,tamBloque);

	int bloqueI = offset/tamBloque, bloqueF = (offset+nbytes-1)/tamBloque, bfisico, bytes = 0;

	int	desp1 = offset%tamBloque; //Desplazamiento en el bloque
	int desp2 = (offset+nbytes-1) % tamBloque;
	int error = 0; //Saber el error de traducir bloque

	if(bloqueI == bloqueF) { //Si son el mismo el bloque inicial y el final
		if((error = traducir_bloque_inodo(inodo,bloqueI,&bfisico,0)) < 0 && error != -2) return bytes;
		else if (error == -2) memset(bufBloque,0,tamBloque);
		else bread(bfisico, bufBloque);
		memcpy (buf, bufBloque+desp1, desp2-desp1+1);
		bytes += desp2-desp1+1;	//Aumentamos el contador los bytes que hemos leido
	} else {
	//3 fases
	//Primer bloque
	if((error = traducir_bloque_inodo(inodo,bloqueI,&bfisico,0)) < 0 && error != -2) return bytes;
	else if (error == -2) memset(bufBloque,0,tamBloque); //Error SOLO CONSULTA DE LA ENTRADA
	else bread(bfisico, bufBloque);
	memcpy (buf, bufBloque+desp1, tamBloque-desp1);
	bytes += tamBloque-desp1;

	//Intermedios
	int bucle;
	for(bucle = bloqueI+1; bucle < bloqueF; bucle++) {
		if((error = traducir_bloque_inodo(inodo,bucle,&bfisico,0)) < 0 && error!= -2) return bytes;
		else if (error == -2) memset(bufBloque,0,tamBloque);
		else bread(bfisico, bufBloque);
		memcpy(buf+(tamBloque-desp1)+(bucle-bloqueI-1)*tamBloque, bufBloque, tamBloque);
		bytes += tamBloque;
	}

	//Ultimo bloque
	if((error = traducir_bloque_inodo(inodo,bloqueF,&bfisico,0)) < 0 && error!= -2) return bytes;
	else if (error == -2) memset(bufBloque,0,tamBloque);
	else bread(bfisico, bufBloque);
	memcpy (buf+(nbytes-desp2-1), bufBloque, desp2+1);
	bytes += desp2+1;
	}
	//Actualizaremos la metainformacion del inodo
	in = leer_inodo(inodo); //Por si el traducir_bloque_Inodo ha reservado inodos
	in.atime = time(NULL);
	if(escribir_inodo(in, inodo) < 0) return -6; //Escribimos el inodo modificado Error ESCRIBIR_INODO

	return bytes;
}

/*
	Tratamiento de errores:
		-2 := Error PERMISOS_INV
		-3 := Error ESCRIBIR_INODO
 */
int mi_chmod_f (unsigned int inodo, unsigned char permisos) {
	struct inodo in;
	in = leer_inodo(inodo);
	if(permisos < 0 || permisos > 7) return -2; // Error PERMISOS_INV
	in.permisos = permisos;
	in.ctime = time(NULL);
	if(escribir_inodo(in, inodo) < 0) return -3; //Escribimos el inodo modificado Error ESCRIBIR_INODO
	return 0;
}

/*
	Tratamiento de errores:
		-2 := Error NO_PERMISOS_ESCRITURA
		-3 := Error LBI (Liberar Bloques Inodo)
		-4 := Error ESCRIBIR_INODO
 */
int mi_truncar_f (unsigned int inodo, unsigned int nbytes) {
	struct inodo in;
	in = leer_inodo(inodo);
	int bytesl;
	if((in.permisos & 2) != 2) return -2; //Error NO_PERMISOS_ESCRITURA
	if(nbytes == 0) bytesl = 0;
	else if(nbytes%tamBloque == 0) bytesl = nbytes/tamBloque;
	else bytesl = nbytes/tamBloque+1;
	if(liberar_bloques_inodo(inodo,bytesl) < 0) return -3; //Error LBI
	in = leer_inodo(inodo);
	in.mtime = time(NULL);
	in.ctime = time(NULL);
	in.tamBytesLogicos = nbytes;
	if(escribir_inodo(in, inodo) < 0) return -4; //Escribimos el inodo modificado Error ESCRIBIR_INODO
	return 0;
}

/*
	Tratamiento de errores:
 */
int mi_stat_f (unsigned int inodo, struct STAT *stat) {
	struct inodo in;
	in = leer_inodo(inodo);

	//Devolvemos la informacion
	stat->tipo = in.tipo;
	stat->permisos = in.permisos;
	stat->atime = in.atime;
	stat->mtime = in.mtime;
	stat->ctime = in.ctime;
	stat->cantLinks = in.cantLinks;
	stat->tamBytesLogicos = in.tamBytesLogicos;
	stat->cantBloquesOcupados = in.cantBloquesOcupados;

	return 0;
}






