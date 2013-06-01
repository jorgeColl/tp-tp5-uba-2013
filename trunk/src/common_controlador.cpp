#include "common_controlador.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

void Controlador::set_directorio(std::string dir){
        this->dir = dir;
}

void Controlador::armar_indice_local() {
        base_de_datos.abrir(dir);
}

list<Modificacion> Controlador::recibir_modificaciones() {
		list<Modificacion> aux;
        return aux;
}

bool Controlador::borrar_archivo(std::string& nombre_archivo)
{
	return base_de_datos.eliminar_archivo(nombre_archivo);
}

bool Controlador::mandar_a_borrar (Modificacion& mod)
{
	/* cambio el "punto de vista" de la modificacion para que cuando el
	server lea esto directamente borre el archivo que tiene en su directorio*/
	mod.accion=BORRAR_ARCHIVO_LOCAL;

	return sock1.enviar_modif(mod);
}

/**
 * @brief Metodo a utilizar para pedir y recibir un archivo del servidor y agregarlo al indice
 * @param mod Moficacion
 * @return
 */
bool Controlador::pedir_nuevo_archivo(Modificacion& mod){
	// Manda mensaje a server pidiendo que transmita archivo
	// lo guarda en un archivo temporal
	// le dice a base de datos que guarde al nuevo archivo

	// cambia el "punto de vista"
	mod.accion =  SUBIR_NUEVO_ARCHIVO;
	sock1.enviar_modif(mod);
	// parte prototipo
	fstream ofstream;
	base_de_datos.abrir_para_escribir_temporal(mod.nombre_archivo, ofstream);
	streampos largo;
	sock1.recibirLen((char*)&largo, sizeof(streampos));
	sock1.recibir_archivo(ofstream);
	ofstream.close();
	base_de_datos.renombrar_temporal(mod.nombre_archivo);
	base_de_datos.registrar_nuevo(mod.nombre_archivo);
	return true;
}

bool Controlador::enviar_nuevo_archivo(std::string& nombre_archivo){
	// No asumo que el archivo entra en ram (en un string)
	// pide a base de datos el archivo como fstream
	// pasa el archivo al server
	sock1.enviar_flag(ARCHIVO_ENTERO);
	// ACA LE TENDRIA QUE PEDIR A LA BASE DE DATOS POR METODO QUE ABRA EL ARCHIVO
	//ifstream fd(nombre_archivo.c_str(),ifstream::binary);
	fstream fd;
	base_de_datos.abrir_para_leer(nombre_archivo, fd);
	sock1.enviar_archivo(fd);
	fd.close();
	return true;
}

bool Controlador::modificar_archivo(std::string& nombre_archivo){
	// le indica a la base de datos que modifique el archivo
	// falta definir parametros de modificacion
	return true;
}
bool Controlador::enviar_modificacion(Modificacion& mod){
	// le manda un mensaje al server indicando que un archivo se modificó y le pasa los datos
	// FALTA DEFINIR QUE DATOS PASA:
	// OP1: PASA NUMERO INI, NUMERO FIN  Y PASA BLOQUE DE ARCHIVO  MODIFICADO
	// OP2: PASA NUMERO INI, NUMERO FIN Y DESP ESPERA A QUE LE PIDAN CIERTO BOLQUE/S
	return true;
}
bool Controlador::pedir_modificacion(std::string& nombre_archivo){
	// falta definir
	return true;
}
bool Controlador::aplicar_modificacion(Modificacion& mod) {
	switch (mod.accion)
	{
	case SUBIR_NUEVO_ARCHIVO:
		return enviar_nuevo_archivo(mod.nombre_archivo);
	case BAJAR_NUEVO_ARCHIVO:
		return pedir_nuevo_archivo(mod);
	case BORRAR_ARCHIVO_LOCAL:
		return borrar_archivo(mod.nombre_archivo);
	case MANDAR_A_BORRAR_ARCHIVO:
		return mandar_a_borrar(mod);
	case SUBIR_MOD_ARCHIVO:
		return enviar_modificacion(mod);
	case BAJAR_MOD_ARCHIVO:
		return pedir_modificacion(mod.nombre_archivo);
	}
	return false;
}

list<Modificacion> Controlador::comprobar_cambios_locales(){
        return base_de_datos.comprobar_cambios_locales();
}

