#!/bin/bash
#     Fichero: scriptprimitivas.sh
#       Autor: Carlos Marin
#       Fecha: 18/06/2014
# Descripcion: Programa cliente. Realiza la lista de funciones primitivas que se le indique 
# 		 Uso: sh scriptprimitivas.sh disco($1)

#Hacemos el make aqui para ir mas rapido y limpiamos la pantalla para empezar
make
clear
#Montamos el disco
echo  Montamos el disco "$1"
./mi_mkfs $1 100000
echo

read -p "Pulsa ENTER para continuar" enter
echo Iniciamos la simulacion de escritura
./simulacion $1
echo
exit

#Hacemos lo mismo que haria el simulacion pero con unos while
echo  Creamos un directorio: /simul_TIMESTAMP/ con permisos 7 
./mi_mkdir $1 7 /simul_TIMESTAMP/
echo
i=0;
while [ $i -lt 100 ]; do
	./mi_mkdir $1 6 /simul_TIMESTAMP/D$i/
	./mi_mkdir $1 6 /simul_TIMESTAMP/D$i/prueba.dat
	j=0;
	while [ $j -lt 50 ]; do
		./escribir  $1 /simul_TIMESTAMP/D$i/prueba.dat $j 
		j=$(($j+1))
	done
    i=$(($i+1))
done

#Listamos la carpeta simul_TIMESTAMP
echo  Mostramos los archivos/carpetas dentro de la directorio: /simul_TIMESTAMP/ 
./mi_ls $1 /simul_TIMESTAMP/
echo
#Listamos la carpeta simul_TIMESTAMP/D0/
echo  Mostramos los archivos/carpetas dentro de la directorio: /simul_TIMESTAMP/D0
./mi_ls $1 /simul_TIMESTAMP/D0/
echo
