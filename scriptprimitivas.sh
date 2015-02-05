#!/bin/bash
#     Fichero: scriptprimitivas.sh
#       Autor: Carlos Marin
#       Fecha: 18/06/2014
# Descripcion: Programa cliente. Realiza la lista de funciones primitivas que se le indique 
# 		  Uso: sh scriptprimitivas.sh disco($1)

#Hacemos el make aqui para ir mas rapido y limpiamos la pantalla para empezar
make
clear
#Montamos el disco
echo  Montamos el disco
./mi_mkfs $1 100000
echo
#Ensenyamos leer_sf
echo  Mostramos los datos del sistema 
./leer_sf $1
echo
#Crear un directorio
echo  Creamos un directorio: /directorio/ con permisos '6' 
./mi_mkdir $1 6 /directorio/
echo

read -p "Pulsa ENTER para continuar" enter
#Listamos la carpeta raiz
echo  Mostramos los archivos/carpetas de la raiz 
./mi_ls $1 /
echo

read -p "Pulsa ENTER para continuar" enter
#Crear un fichero
echo  Creamos un fichero dentro del directorio: /directorio/fichero.txt con permisos '6' 
./mi_mkdir $1 6 /directorio/fichero.txt
echo

read -p "Pulsa ENTER para continuar" enter
#Listamos la carpeta directorio
echo  Mostramos los archivos/carpetas dentro de la directorio: /directorio/ 
./mi_ls $1 /directorio/
echo

read -p "Pulsa ENTER para continuar" enter
#Ecribimos en el fichero
echo  Escribimos en el fichero.txt 
./escribir $1 /directorio/fichero.txt 0
echo

read -p "Pulsa ENTER para continuar" enter
#Leemos del fichero y direccionamos
echo  Leemos desde el fichero direccionado la salida a fichero.txt 
./mi_cat $1 /directorio/fichero.txt > fichero.txt
echo

read -p "Pulsa ENTER para continuar" enter
#Creamos un directorio para llenarlo con 17 directorios
echo  Creamos un directorio para que contenga 17 directorios 
./mi_mkdir $1 6 /directorio17/
echo

read -p "Pulsa ENTER para continuar" enter
echo  Creamos los 17 directorios
i=0;
while [ $i -lt 17 ]; do
	./mi_mkdir $1 6 /directorio17/D$i/
    i=$(($i+1))
done
echo

read -p "Pulsa ENTER para continuar" enter
#Listamos la carpeta directorio
echo  Mostramos los archivos/directorios de directorio17 
./mi_ls $1 /directorio17/
echo

read -p "Pulsa ENTER para continuar" enter
#Mi_stat de directorio17
echo  Mostramos el STAT del directorio directorio17 
./mi_stat $1 /directorio17/
echo
echo Vemos que ocupa 2 Bloques
echo

read -p "Pulsa ENTER para continuar" enter
#Mi_rm de D0
echo  Borramos el directorio D0 
./mi_rm $1 /directorio17/D0
echo

read -p "Pulsa ENTER para continuar" enter
#Mi_stat de directorio17
echo  Mostramos el STAT del directorio directorio17 
./mi_stat $1 /directorio17/
echo

echo Ahora ocupa solo 1 bloque
echo

read -p "Pulsa ENTER para continuar" enter
#Mi_chmod de Directorio17
echo  Cambiamos permisos del directorio17 de 6 a 2 
./mi_chmod $1 2 /directorio17/
echo

read -p "Pulsa ENTER para continuar" enter
#Listamos la carpeta raiz
echo  Mostramos el contenido de la raiz 
./mi_ls $1 /
echo
echo Ahora vemos como tiene permisos 2
echo

read -p "Pulsa ENTER para continuar" enter
echo  Enlazamos a fichero.txt en directorio/ficheroln.txt en raiz 
./mi_ln $1 /directorio/fichero.txt /ficheroln.txt
echo 

read -p "Pulsa ENTER para continuar" enter
echo  Mostramos el STAT de fichero.txt 
./mi_stat $1 /directorio/fichero.txt
echo
echo Vemos como ahora tiene 2 links
echo

read -p "Pulsa ENTER para continuar" enter
echo  Mostramos el STAT de ficheroln.txt 
./mi_stat $1 /ficheroln.txt
echo 

echo Vemos que es igual que el anterior
echo
read -p "Pulsa ENTER para continuar" enter
echo  Borramos fichero.txt 
./mi_rm $1 /directorio/fichero.txt
echo

read -p "Pulsa ENTER para continuar" enter
echo  Mostramos el STAT ficheroln.txt 
./mi_stat $1 /ficheroln.txt
echo
echo Vemos como ahora la cantidad de links es 1
