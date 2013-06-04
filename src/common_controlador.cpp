#include "common_controlador.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>


using namespace std;

Controlador::Controlador() : dir(), sock1(), sock2(), conectado(false) {}

Controlador::Controlador(const string &dir, int sockfd1, int sockfd2)
	: dir(dir), sock1(sockfd1), sock2(sockfd2), conectado(false) {}

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

bool Controlador::borrar_archivo(const string &nombre_archivo)
{
	bool exito = base_de_datos.eliminar_archivo(nombre_archivo);
	if (!exito) return false;
	base_de_datos.registrar_eliminado(nombre_archivo);
	return true;
}

bool Controlador::copiar_archivo(const string &nombre_archivo, const string &nombre_archivo_anterior)
{
	bool exito = base_de_datos.copiar(nombre_archivo_anterior, nombre_archivo);
	if (!exito) return false;
	base_de_datos.registrar_copiado(nombre_archivo, nombre_archivo_anterior);
	return true;
}

bool Controlador::renombrar_archivo(const string &nombre_archivo, const string &nombre_archivo_anterior)
{
	bool exito = base_de_datos.renombrar(nombre_archivo_anterior, nombre_archivo);
	if (!exito) return false;
	base_de_datos.registrar_renombrado(nombre_archivo, nombre_archivo_anterior);
	return true;
}

/**
 * @brief Metodo a utilizar para pedir y recibir un archivo del servidor y agregarlo al indice
 * @param mod Moficacion
 * @return
 */
bool Controlador::pedir_nuevo_archivo(string& nombre_archivo){
	// Manda mensaje a server pidiendo que transmita archivo
	// Le dice a base de datos que guarde al nuevo archivo
	sock1.enviar_flag(PEDIDO_ARCHIVO_ENTERO);
	sock1.enviar_msg_c_prefijo(nombre_archivo, 1);
	cout << "bien" << endl;
	recibir_nuevo_archivo(nombre_archivo);
	cout << "bien2" << endl;
	base_de_datos.registrar_nuevo(nombre_archivo);
	cout << "bien3" << endl;
	return true;
}

bool Controlador::enviar_nuevo_archivo(std::string& nombre_archivo)
{
	ifstream fd;
	bool exito = base_de_datos.abrir_para_leer(nombre_archivo, fd);
	if (!exito) return false;
	sock1.enviar_archivo(fd);
	fd.close();
	PacketID flag;
	sock1.recibir_flag(flag);
	if (flag == OK)
	{
		base_de_datos.registrar_nuevo(nombre_archivo);
		return true;
	}
	return false; // Algo fallo?
}

bool Controlador::recibir_nuevo_archivo(const string &nombre_archivo)
{
	ofstream ofstream;
	base_de_datos.abrir_para_escribir(nombre_archivo, ofstream);
	sock1.recibir_archivo(ofstream);
	ofstream.close();
	base_de_datos.registrar_nuevo(nombre_archivo);
	return true;
}

bool Controlador::modificar_archivo(std::string& nombre_archivo){
	// le indica a la base de datos que modifique el archivo
	// falta definir parametros de modificacion
	return true;
}

bool Controlador::enviar_edicion(Modificacion& mod){
	//TODO : Terminar
	return true;
}

bool Controlador::pedir_edicion(std::string& nombre_archivo){
	// falta definir
	return true;
}

bool Controlador::aplicar_modificacion(Modificacion& mod)
{
	if (mod.es_local) // Si es local tengo que comunicarle al servidor del cambio
	{
		sock1.enviar_flag(MODIFICACION);
		sock1.enviar_modif(mod);
		PacketID flag;
		sock1.recibir_flag(flag);
		if (flag == YA_APLICADA) return true;
		if (flag != OK) return false; // Algo fallo?
	}
	switch (mod.accion)
	{
		case NUEVO:
			if (mod.es_local) return enviar_nuevo_archivo(mod.nombre_archivo);
			else return pedir_nuevo_archivo(mod.nombre_archivo);
		case EDITADO:
			if (mod.es_local) return enviar_edicion(mod);
			else return pedir_edicion(mod.nombre_archivo);
		case BORRADO:
			if (mod.es_local)
			{
				PacketID flag;
				sock1.recibir_flag(flag);
				cout << "flag: " << flag << endl;
				if (flag != OK) return false;
				base_de_datos.registrar_eliminado(mod.nombre_archivo);
				return true;
			}
			else return borrar_archivo(mod.nombre_archivo);
		case RENOMBRADO:
			if (mod.es_local)
			{
				PacketID flag;
				sock1.recibir_flag(flag);
				if (flag != OK) return false;
				base_de_datos.registrar_renombrado(mod.nombre_archivo, mod.nombre_archivo_alt);
				return true;
			}
			else return renombrar_archivo(mod.nombre_archivo, mod.nombre_archivo_alt);
		case COPIADO:
			if (mod.es_local)
			{
				PacketID flag;
				sock1.recibir_flag(flag);
				if (flag != OK) return false;
				base_de_datos.registrar_copiado(mod.nombre_archivo, mod.nombre_archivo_alt);
				return true;
			}
			else return copiar_archivo(mod.nombre_archivo, mod.nombre_archivo_alt);
		default: // En realidad no existen otros casos
			return true;
	}
	return false;
}

list<Modificacion> Controlador::comprobar_cambios_locales(){
        return base_de_datos.comprobar_cambios_locales();
}

