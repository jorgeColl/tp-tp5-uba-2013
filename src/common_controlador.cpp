#include "common_controlador.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

void Controlador::set_directorio(std::string dir){
	this->dir = dir;
}
bool Controlador::armar_indice_local() {
	return base_de_datos.abrir(dir);
}

vector<Modificacion> Controlador::recibir_modificaciones() {
	vector<Modificacion> aux;
	return aux;
}
bool Controlador::borrar_archivo(std::string& nombre_archivo){
	return base_de_datos.eliminar_archivo(nombre_archivo);
}
bool Controlador::mandar_a_borrar (Modificacion& mod) {
	/* cambio el "punto de vista" de la modificacion para que cuando el
	server lea esto directamente borre el archivo que tiene en su directorio*/
	mod.accion=BORRAR_ARCHIVO_LOCAL;

	return sock1.enviar_modif(mod);
}
bool Controlador::pedir_nuevo_archivo(Modificacion& mod){
	// manda mensaje a server pidiendo que transmita archivo
	// lo guarda temporalmente en un archivo
	// le dice a base de datos que guarde al nuevo archivo

	// cambia el "punto de vista"
	mod.accion =  SUBIR_NUEVO_ARCHIVO;
	sock1.enviar_modif(mod);
	// parte prototipo
	ofstream* fd = base_de_datos.generar_archivo_temp(mod.nombre_archivo);
	sock1.recibir_archivo(*fd);
	base_de_datos.cerrar_exitosamente_archivo(mod.nombre_archivo,*fd);
	return true;
}
bool Controlador::enviar_nuevo_archivo(std::string& nombre_archivo){
	// asumo que el archivo entra en ram (en un string)
	// pide a base de datos el archivo en string
	// pasa el archivo (que está en forma de string) al server
	sock1.enviar_flag(ARCH_NUEVO);
	// ACA LE TENDRIA QUE PEDIR A LA BASE DE DATOS POR METODO QUE ABRA EL ARCHIVO
	//ifstream fd(nombre_archivo.c_str(),ifstream::binary);
	ifstream* fd = base_de_datos.abrir_para_leer(nombre_archivo);
	sock1.enviar_archivo(*fd);
	fd->close();
	delete fd;
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
	switch (mod.accion) {
	case SUBIR_NUEVO_ARCHIVO: {
		return enviar_nuevo_archivo(mod.nombre_archivo);
		break;
	}
	case BAJAR_NUEVO_ARCHIVO: {
		return pedir_nuevo_archivo(mod);
		break;
	}
	case BORRAR_ARCHIVO_LOCAL: {
		return borrar_archivo(mod.nombre_archivo);
		break;
	}
	case MANDAR_A_BORRAR_ARCHIVO: {
		return mandar_a_borrar(mod);
		break;
	}
	case SUBIR_MOD_ARCHIVO: {
		return enviar_modificacion(mod);
		break;
	}
	case BAJAR_MOD_ARCHIVO: {
		return pedir_modificacion(mod.nombre_archivo);
		break;
	}
	}
	return false;
}


std::vector<Modificacion> Controlador::comprobar_cambios_locales(){
	return base_de_datos.comprobar_cambios_locales();
}


