#include "common_controlador.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

Controlador::Controlador() : dir(), sock1(), sock2(), terminar(false) {}

Controlador::Controlador(const string &dir, int sockfd1, int sockfd2)
	: dir(dir), sock1(sockfd1), sock2(sockfd2), terminar(false) {}

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
	//Este metodo como que no hace mucho
	/* cambio el "punto de vista" de la modificacion para que cuando el
	server lea esto directamente borre el archivo que tiene en su directorio*/
	//mod.accion=BORRAR_ARCHIVO_LOCAL;

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

bool Controlador::enviar_nuevo_archivo(std::string& nombre_archivo)
{
	sock1.enviar_flag(ARCHIVO_ENTERO);
	fstream fd;
	bool exito = base_de_datos.abrir_para_leer(nombre_archivo, fd);
	if (!exito) return false;
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
	string modSerializada(mod.serializar());
	sock1.enviar_flag(MODIFICACION);
	//TODO : Terminar
	return true;
}

bool Controlador::pedir_modificacion(std::string& nombre_archivo){
	// falta definir
	return true;
}

bool Controlador::aplicar_modificacion(Modificacion& mod)
{
	switch (mod.accion)
	{
	case NUEVO:
		if (mod.es_local) return enviar_nuevo_archivo(mod.nombre_archivo);
		else return pedir_nuevo_archivo(mod);
	case BORRADO:
		if (mod.es_local) return borrar_archivo(mod.nombre_archivo);
		else return mandar_a_borrar(mod);
	case MODIFICADO:
		if (mod.es_local) return enviar_modificacion(mod);
		else return pedir_modificacion(mod.nombre_archivo);
	case RENOMBRADO:
		return true;
	case COPIADO:
		return true;
	}
	return false;
}

list<Modificacion> Controlador::comprobar_cambios_locales(){
        return base_de_datos.comprobar_cambios_locales();
}

