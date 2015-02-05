/*
     Fichero: verificacion.c
       Autor: Carlos Marin
       Fecha: 21/06/2014
 Descripcion: Programa cliente. Cuando acaben las escrituras, se verificara el fichero "prueba.dat" de cada proceso.
 		 Uso: ./verificacion disco directorio_simulacion
 */
#include "./src/registro.h"
#include "./src/directorios.h"

struct informacion {
	int proceso; //Escribir el PID
	unsigned int nEscrituras; //Escribir el contador de los registros validados dentro del fichero "prueba.dat"
	struct registro PrimeraEscritura; //Escribir la menor fecha y hora en formato legible, el numero de escritura en que ocurrio y su posicion
	struct registro UltimaEscritura; //Escribir la mayor fecha y hora en formato legible, el numero de escritura en que ocurrio y su posicion
	struct registro MenorPosicion; //Escribir la posicion mas baja, el numero de escritura en que ocurrio y su fecha y hora en formato legible
	struct registro MayorPosicion; //Escribir la posicion mas alta, el numero de escritura en que ocurrio y su fecha y hora en formato legible
};

void Rbuit(struct registro * reg) {
	//Inicializar todos los registros a 0
	reg->fecha = 0;
	reg->pid = 0;
	reg->nEscritura = 0;
	reg->posicion = 0;
}

void mostrarRegistro(struct registro *reg) {
	char fec[20];
	memset(fec,0,20);
	struct tm *ts;
	time_t t = reg->fecha;
	ts = localtime(&t);
	strftime(fec, sizeof(fec), "%Y/%m/%d %H:%M:%S", ts);
	printf("\t· Fecha: %s\n", fec);
	printf("\t· Numero escritura: %d\n", reg->nEscritura);
	printf("\t· Posicion: %d\n", reg->posicion);
}

void mostrarInfo(struct informacion *info) {
	printf("PID: %d\n", info->proceso);
	printf("Registros validados: %d\n", info->nEscrituras);
	puts("Primera Escritura:");
	mostrarRegistro(&info->PrimeraEscritura);
	puts("Ultima Escritura:");
	mostrarRegistro(&info->UltimaEscritura);
	puts("Mayor Posicion:");
	mostrarRegistro(&info->MayorPosicion);
	puts("Menor Posicion:");
	mostrarRegistro(&info->MenorPosicion);
}

int main (int argc, char **argv) {
	if (argc != 3) {
		printf("Numero de parametros incorrectos\n");
		return -1;
	}
	bmount(argv[1]); //Montamos el sistema de ficheros

	int tamRegistros = (tamBloque/sizeof(struct registro))*200; //Numero de registros que lee de golpe
	int numeroEntradas = 100; //El numero de entradas que tiene el directorio
	struct entrada ent[numeroEntradas];
	char *pid;

	struct registro PrimeraEscritura, UltimaEscritura, MayorPosicion, MenorPosicion;
	struct registro registros[tamRegistros];
	struct registro Intermediario;
	struct informacion info;
	char ruta[200];
	memset(ruta, 0, 200);

	struct STAT stat;
	if(mi_stat(argv[2], &stat) < 0){
		puts("Error a la hora de obtener el STAT del directorio de simulacion");
		return -1; //Error en el MI_STAT
	}

	if(stat.tamBytesLogicos/sizeof(struct entrada) < numeroEntradas){ //Calcular el num de entradas, que ha de ser 100
		puts("Deben existir 100 entradas en el directorio");
		return -1;
	}

	char informe [200]; //Para crear el fichero
	char *camino = argv[2]; //Contendra el directorio de simulacion
	sprintf(informe, "%sinforme.txt", camino);
	//Crear el fichero informe.txt dentro del directorio de simulacion
	if(mi_creat(informe, 6) < 0){
		puts("Error a la hora de crear el informe.txt");
		return -1;
	}

	if(mi_read(camino, &ent, 0, sizeof(struct entrada)*numeroEntradas) < 0){ //Leemos las 100 entradas que tiene que tener el fichero
		printf("ERROR a la hora de leer las entradas de %s\n", camino);
		return -1;
	}

	int entrada, siguiente, offset, nescrituras, npid, offsetInforme, i, tInfo = sizeof(struct informacion);
	for(entrada = 0, offsetInforme = 0; entrada < 100; entrada++, offsetInforme += tInfo){ //Para cada entrada del directorio de simulacion
		pid = strchr(ent[entrada].nombre,'_') + 1; //Extraer el pid a partir de su nombre
		npid = atoi(pid); //Lo casteamos a int
		sprintf(ruta, "%s/%s/prueba.dat", camino, ent[entrada].nombre);

		Rbuit(&PrimeraEscritura);
		Rbuit(&UltimaEscritura);
		Rbuit(&MayorPosicion);
		Rbuit(&MenorPosicion);

		siguiente = mi_read(ruta, &registros, 0, tamRegistros); //Leemos siguientes registro
		for(offset = tamRegistros, nescrituras = 0; siguiente > 0; offset += tamRegistros){ //Mientras siguiente > 0, ergo, no problemas de lectura
			for(i = 0; i < tamRegistros; i++){ //Miramos todos los registros leidos
				Intermediario = registros[i];
				if(Intermediario.pid == npid){ //Si el pid coincide
					nescrituras++;
					if(nescrituras == 1){ //Si es la primera escritura
						PrimeraEscritura = Intermediario;
						UltimaEscritura = Intermediario;
						MenorPosicion = Intermediario;
					}
					if(PrimeraEscritura.fecha > Intermediario.fecha){
						PrimeraEscritura = Intermediario;
					}
					if(UltimaEscritura.fecha < Intermediario.fecha){
						UltimaEscritura = Intermediario;
					}
					MayorPosicion = Intermediario; //La mayor posicion sera la ultima que leamos
				}
			}
			siguiente = mi_read(ruta, &registros, offset, tamRegistros); //Leemos siguiente registro
		}
		//Actualizamos los datos del struct informacion
		info.proceso = npid;
		info.nEscrituras = nescrituras;
		info.PrimeraEscritura = PrimeraEscritura;
		info.UltimaEscritura = UltimaEscritura;
		info.MayorPosicion = MayorPosicion;
		info.MenorPosicion = MenorPosicion;

		//Escribir en informe.txt los datos que hemos recogido
		if(mi_write(informe, &info, offsetInforme, tInfo)< 0){
			puts("ERROR: Error al escribir la struct INFO");
			return 0;
		}
		mostrarInfo(&info); //Tambien lo mostramos por pantalla
	}
	bumount(); //Desmontamos el sistema de ficheros
}
