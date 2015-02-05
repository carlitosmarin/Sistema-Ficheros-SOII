/*  
     Fichero: registro.h
       Autor: Carlos Marin
       Fecha: 21/06/2014
 Descripcion: Cabecera en la que declararemos la estructura Registro, como la usaran tanto Simulacion como Verificacion, la hacemos separada.
 */

struct registro {
	unsigned int fecha; //Entero (time_t) con la fecha y hora en forma epoch
	int pid; //Entero con el PID del proceso que lo creo
	int nEscritura; //Entero con el numero de escritura (de 1 a 50)
	int posicion; //Entero con la posicion del registro dentro del fichero
};
