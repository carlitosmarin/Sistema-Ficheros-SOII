/*
     Fichero: directorios.c
       Autor: Carlos Marin
       Fecha: 08/06/2014
 Descripcion: Funciones de tratamiento basico de las direcciones del sistema de ficheros
 */

#include "directorios.h"

/*
	Tratamiento de errores:
		-1 := Error CAMINO_INV
 */
int extraer_camino (const char *camino, char *inicial, char *final) {
	int i = 1, f = 0;

	if (camino[0]!='/') return -1; //Error CAMINO_INV

	//Para obtener el directorio inicial
	while ((camino[i] != '/') && (camino[i] != '\0')){
		inicial[i-1] = camino[i];
		i++;
	}

	if (i == strlen(camino)) { //Se trata de un fichero
		final = "\0";
		return 1; // FICHERO
	}

	while (i < strlen(camino)) {
		final[f] = camino[i];
		i++; f++;
	}
	return 0; //DIRECTORIO
}

/*
	La funcion buscar_entrada dada una cadena de caracteres (camino_parcial) y el inodo de directorio sobre el que se apoya esta cadena (p_inodo_dir), calcula:
	- El numero de inodo de su directorio más cercano (p_inodo_dir).
	- Su numero de inodo (p_inodo).
	- El numero de su entrada dentro del ultimo directorio que lo contiene (p_entrada).

	Tratamiento de errores:
		-1 := Error EC (Extraer Camino)
		-2 := Error NO_PERMISOS_LECTURA
		-3 := Error LECTURA
		-4 := Error NO_ENTRADA_CONSULTA
		-5 := Error RESERVAR_INODO
		-6 := Error NO_DIRECTORIO_INTERMEDIO
		-7 := Error LIBERAR_INODO
		-8 := Error ESCRITURA
	    -9 := Error YA_EXISTE_ENTRADA
 */
int buscar_entrada (const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos) {
	if(strcmp(camino_parcial,"/") == 0) { //El directorio es raiz "/"
		*p_inodo = 0; //Raiz asignada al inodo 0
		*p_entrada = 0;
		return 0;
	}

	char inicial[longEntrada], final[strlen(camino_parcial)];
	memset(inicial, 0, longEntrada);
	memset(final, 0, strlen(camino_parcial));
	struct inodo in;
	int EC;
	if((EC = extraer_camino(camino_parcial,inicial,final)) < 0) return -1; //ERROR EC
	//Buscamos la entrada cuyo nombre se encuentra en inicial
	in = leer_inodo(*p_inodo_dir);
	struct entrada entr;
	entr.nombre[0] = '\0';

	int numEntr = in.tamBytesLogicos/sizeof(struct entrada);
	int nentrada = 0; //Num. entrada inicial
	if(numEntr > 0) {
		if((in.permisos & 4) != 4) return -2; //Error NO_PERMISOS_LECTURA
		if(mi_read_f(*p_inodo_dir,&entr,nentrada*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -3; //Error LECTURA
		while(nentrada < numEntr && strcmp(inicial,entr.nombre)!= 0) {
			nentrada++;
			if(mi_read_f(*p_inodo_dir,&entr,nentrada*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -3; //Error LECTURA
		}
	}
	if(nentrada == numEntr) { //Ha salido del bucle pq no quedaban mas entradas, no por haber encontrado la deseada
		switch(reservar) {
		case 0: //Modo consulta, como no existe retornamos error.
			return -4; //Error NO_ENTRADA_CONSULTA
			break;
		case 1: //Modo escritura, creamos la entrada de directorio en el directorio referenciado
			strcpy(entr.nombre, inicial);
			int inR;
			if(EC == 0) { //DIRECTORIO
				if(strcmp(final,"/") == 0) {
					if((inR = reservar_inodo('d', permisos)) < 0) return -5; //Error RERSERVAR_INODO
				}
				else return -6; //Error NO_DIRECTORIO_INTERMEDIO
			} else {
				if((inR = reservar_inodo('f', permisos)) < 0) return -5; //FICHERO Error RESERVAR_INODO
			}
			entr.inodo = inR;
			if(mi_write_f(*p_inodo_dir,&entr,nentrada*sizeof(struct entrada), sizeof(struct entrada)) < 0) {
				//Si hay error, liberamos el inodo y devolvemos error
				if(liberar_inodo(entr.inodo) < 0) return -7; //Error LIBERAR_INODO
				return -8;
			}
		}
	}
	if(strcmp(final,"/") == 0 || EC == 1) { //Hemos llegado al final del camino o es un fichero
		if((nentrada < numEntr) && (reservar == 1)) return -9; //ERROR Entrada ya existente
		*p_inodo = entr.inodo;
		*p_entrada = nentrada;
	} else { //Si no es final extraemos de nuevo el camino
		*p_inodo_dir = entr.inodo;
		switch(buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos)) {
			case -1: return -1;
			case -2: return -2;
			case -3: return -3;
			case -4: return -4;
			case -5: return -5;
			case -6: return -6;
			case -7: return -7;
			case -8: return -8;
			case -9: return -9; //Devolvemos todo tipo de error de las llamadas recursivas
		}
	}
	return 0;
}

/*
	Crea un directorio/fichero
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
 */
int mi_creat (const char *camino, unsigned char permisos) {
	unsigned int pid = 0, pi = 0, pe = 0;
	mi_waitSem();
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 1, permisos);
	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		puts("Falla MI_CREAT");
		mi_signalSem();
		return -1; //Error BE
	}
	mi_signalSem();
	return 0;
}

/*
	Pone el contenido del directorio en un buffer de memoria
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error LECTURA
 */
int mi_dir (const char *camino, char *buf) {
	int pid = 0, pi = 0, pe = 0;
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		puts("Falla MI_DIR");
		return -1; //Error BE
	}
	struct inodo in;
	in = leer_inodo(pi);
	struct entrada entr;
	if(in.tipo != 'd' && in.permisos & 4) return -1;
	int numEntradas = in.tamBytesLogicos/sizeof(struct entrada);
	int nentrada = 0;
	while(nentrada < numEntradas){ //Mientras haya entradas
			if(mi_read_f(pi,&entr,nentrada*sizeof(struct entrada),sizeof(struct entrada)) < 0) return -2; //Leemos una entrada Error LECTURA
			in = leer_inodo(entr.inodo);

			if(in.tipo == 'd') strcat(buf,"D"); //Tipo de inodo
			else strcat(buf,"F");
			strcat(buf,"\t");
			//Para incorporar informacion acerca de los permisos
			if(in.permisos & 4) strcat(buf,"r");
			else strcat(buf,"-");

			if(in.permisos & 2) strcat(buf,"w");
			else strcat(buf,"-");

			if(in.permisos & 1) strcat(buf,"x");
			else strcat(buf,"-");

			strcat(buf, "\t");

			//Para incorporar informacion acerca del tiempo
			struct tm *tm;
			char tmp[100];
			tm = localtime(&in.mtime);
			sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
			strcat(buf,tmp);

			strcat(buf,entr.nombre);
			strcat(buf,"\n");
			nentrada++;
		}
	return numEntradas;
}

/*
	Crea el enlace de una entrada de directorio camino2 al inodo especificado por otra entrada de directorio camino1
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error DIRECTORIO_NO_ENLAZA
		-3 := Error LECTURA
		-4 := Error ESCRITURA
		-5 := Error ESCRIBIR_INODO
 */
int mi_link (const char *camino1, const char *camino2) {
	int pid = 0, pi = 0, pe = 0;
	int origen = buscar_entrada(camino1, &pid, &pi, &pe, 0, 0);

	if(origen < 0) { //Tratamiento de errores
		switch(origen){
		case -1: printf("\"%s\" Camino no valido\n", camino1); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino1); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino1); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino1); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino1); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino1); break;
		}
		return -1; //Error BE
	}
	int inodo = pi;
	struct inodo in = leer_inodo(inodo);
	if(in.tipo != 'f') return -2; //Error, es un directorio Error DIRECTORIO_NO_ENLAZA

	pid = 0, pi = 0, pe = 0;
	mi_waitSem();
	int destino = buscar_entrada(camino2, &pid, &pi, &pe, 1, 6);
	mi_signalSem();
	if(destino < 0) { //Tratamiento de errores
		switch(destino){
		case -1: printf("\"%s\" Camino no valido\n", camino2); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino2); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino2); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino2); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino2); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino2); break;
		}
		return -1; //Error BE
	} //Entrada creada con exito
	struct entrada entr;
	if(mi_read_f(pid,&entr,pe*sizeof(struct entrada), sizeof(struct entrada)) < 0) return -3; //Error LECTURA
	mi_waitSem();
	liberar_inodo(pi); //Liberamos el inodo que se ha asociado a la entrada
	entr.inodo = inodo;
	if(mi_write_f(pid,&entr,pe*sizeof(struct entrada), sizeof(struct entrada)) < 0) return -4; //Error ESCRITURA
	mi_signalSem();

	in.cantLinks++; //Actualizamos la informacion de la cantidad de enlaces
	in.ctime = time(NULL); //Actualizamos la informacion del ctime
	mi_waitSem();
	escribir_inodo(in, inodo);
	mi_signalSem();
	return 0;
}

/*
	Borra la entrada de directorio especificada (no hay que olvidar actualizar la cantidad de enlaces de entradas en directorio del inodo) y, en caso de que sea el último enlace existente, borrar el propio fichero/directorio.
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error LECTURA
		-3 := Error ESCRITURA
		-4 := Error TRUNCAR
		-5 := Error ESCRIBIR_INODO
		-6 := Error LIBERAR_INODO
 */
int mi_unlink (const char *camino) {
	int pid = 0, pi = 0, pe = 0;
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);
	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		return -1; //Error BE
	} else {
		struct inodo in;
		in = leer_inodo(pid);
		int nentradas = in.tamBytesLogicos/sizeof(struct entrada);

		//Si no es la ultima entrada
		if(nentradas-1 != pe) {
			struct entrada entr;
			if(mi_read_f(pid, &entr, (nentradas-1) * sizeof(struct entrada), sizeof(struct entrada)) < 0) return -2; //Error LECTURA
			mi_waitSem();
			mi_write_f(pid,&entr, pe*sizeof(struct entrada), sizeof(struct entrada)); //Error ESCRITURA
			mi_signalSem();
		}
		mi_waitSem();
		if(mi_truncar_f(pid,(nentradas-1)*sizeof(struct entrada)) < 0) return -4; //Error TRUNCAR
		mi_signalSem();

		in = leer_inodo(pi);

		//Si no tiene mas links, borramos el el propio fichero o directorio
		if(in.cantLinks == 1) {
			mi_waitSem();
			if(liberar_inodo(pi) < 0) return -6; //Error LIBERAR_INODO
			mi_signalSem();
		} else { //Si no, actualizamos
			in.ctime = time(NULL);
			in.cantLinks--; //Restamos el link que hemos quitado
			mi_waitSem();
			if(escribir_inodo(in, pi) < 0)return -5; //Escribimos el inodo Error ESCRIBIR_INODO
			mi_signalSem();
		}
	}
	return 0;
}

/*
	Cambia los permisos de una entrada camino
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error CHMOD
 */
int mi_chmod (const char *camino, unsigned char permisos) {
	int pid = 0, pi = 0, pe = 0;
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);

	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		return -1; //Error BE
	} else {
		if(mi_chmod_f(pi,permisos) < 0) return -2; //Error CHMOD
	}
	return 0;
}

/*
	Transifere todo la informacion de un inodo a un stat
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error STAT
 */
int mi_stat (const char *camino, struct STAT *p_stat) {
	int pid = 0, pi = 0, pe = 0;
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);

	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		return -1; //Error BE
	} else {
		if(mi_stat_f(pi,p_stat) < 0) return -2; //Error STAT
	}
	return 0;
}

/*
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error READ
 */
int mi_read (const char *camino, void *buf, unsigned int offset, unsigned int nbytes) {
	int pid = 0, pi = 0, pe = 0, bytesLeidos;
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 0, 0);

	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		puts("Falla MI_READ");
		return -1; //Error READ
	} else {
		if((bytesLeidos = mi_read_f(pi,buf,offset,nbytes)) < 0) return -2; //Error READ
	}
	return bytesLeidos;
}

/*
	Tratamiento de errores:
		-1 := Error BE (Buscar Entrada)
		-2 := Error WRITE
		-3 := Error LECTURA_DIRECTORIO
 */
int mi_write (const char *camino, const void *buf, unsigned int offset, unsigned int nbytes) {
	int pid = 0, pi = 0, pe = 0, bytesLeidos;
	mi_waitSem();
	int BE = buscar_entrada(camino, &pid, &pi, &pe, 0, 6);
	if(BE < 0) { //Tratamiento de errores
		switch(BE){
		case -1: printf("\"%s\" Camino no valido\n", camino); break;
		case -2: printf("No tienes permisos de lectura\n"); break;
		case -3: printf("Error en la lectura de la entrada \"%s\"\n", camino); break;
		case -4: printf("No existe la entrada \"%s\"\n", camino); break;
		case -5: printf("Error a la hora de reservar un inodo"); break;
		case -6: printf("No existe directorio intermedio a \"%s\"", camino); break;
		case -7: printf("Error a la hora de liberar un inodo"); break;
		case -8: printf("Error en la escritura de la entrada \"%s\"\n", camino); break;
		case -9: printf("Ya existe la entrada \"%s\"\n", camino); break;
		}
		mi_signalSem();
		puts("Falla MI_WRITE");
		return -1; //Error READ
	}
	struct inodo in = leer_inodo(pi);
	if(in.tipo != 'f'){
		puts("No se puede leer un directorio."); //Error LECTURA_DIRECTORIO
		mi_signalSem();
		return -3;
	}

	if((bytesLeidos = mi_write_f(pi,buf,offset,nbytes)) < 0) return -2; //Error WRITE
	mi_signalSem();
	return bytesLeidos;
}
