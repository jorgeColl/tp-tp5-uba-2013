#include "common_controlador.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>	// Memcpy
#include "common_util.h"
#include "common_hashing.h"
#include <syslog.h>

using namespace std;

Controlador::Controlador() : dir(), sock1(), sock2(), conectado(false) {}

Controlador::Controlador(const string &dir, int sockfd1, int sockfd2, const string &password)
	: dir(dir), sock1(sockfd1), sock2(sockfd2), password(password), conectado(false) {}

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
	if (esArchivo(dir, nombre_archivo)) // Solo tiene sentido si el archivo existe
	{
		bool exito = base_de_datos.eliminar_archivo(nombre_archivo);
		if (!exito) return false;
	}
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

bool Controlador::pedir_nuevo_archivo(const string& nombre_archivo){
	// Manda mensaje a server pidiendo que transmita archivo
	// Le dice a base de datos que guarde al nuevo archivo
	sock1.enviar_flag(PEDIDO_ARCHIVO_ENTERO);
	sock1.enviar_msg_c_prefijo(nombre_archivo, BYTES_PREF_NOMBRE);
	recibir_nuevo_archivo(nombre_archivo);
	base_de_datos.registrar_nuevo(nombre_archivo);
	return true;
}

bool Controlador::enviar_nuevo_archivo(const string &nombre_archivo)
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

bool Controlador::enviar_edicion(const Modificacion& mod)
{
	ifstream archivo;
	base_de_datos.abrir_para_leer(mod.nombre_archivo, archivo);
	sock1.enviar_edicion(archivo);
	archivo.close();
	PacketID flag;
	sock1.recibir_flag(flag);
	if (flag != OK) return false;
	base_de_datos.registrar_editado(mod.nombre_archivo);
	return true;
}

bool Controlador::pedir_edicion(const string& nombre_archivo)
{
	sock1.enviar_flag(PEDIDO_ARCHIVO_EDICIONES);
	sock1.enviar_msg_c_prefijo(nombre_archivo, BYTES_PREF_NOMBRE);
	ofstream destino;
	base_de_datos.abrir_para_escribir_temporal(nombre_archivo, destino);
	ifstream original;
	base_de_datos.abrir_para_leer(nombre_archivo, original);
	sock1.recibir_edicion(original, destino);
	// Terminamos
	original.close();
	destino.close();
	bool exito = base_de_datos.renombrar_temporal(nombre_archivo);
	if (exito) base_de_datos.registrar_editado(nombre_archivo);
	else
	{
		base_de_datos.eliminar_archivo_temporal(nombre_archivo);
		return false;
	}
	return true;
}

bool Controlador::enviar_borrado(const string& nombre_archivo)
{
	PacketID flag;
	sock1.recibir_flag(flag);
	if (flag != OK) return false;
	base_de_datos.registrar_eliminado(nombre_archivo);
	return true;
}

bool Controlador::enviar_renombrado(const string& nombre_archivo_nuevo,
		const string& nombre_archivo_viejo)
{
	PacketID flag;
	sock1.recibir_flag(flag);
	if (flag != OK) return false;
	base_de_datos.registrar_renombrado(nombre_archivo_nuevo, nombre_archivo_viejo);
	return true;
}

bool Controlador::enviar_copiado(const string& nombre_archivo_nuevo,
		const string& nombre_archivo_viejo)
{
	PacketID flag;
	sock1.recibir_flag(flag);
	if (flag != OK) return false;
	base_de_datos.registrar_copiado(nombre_archivo_nuevo, nombre_archivo_viejo);
	return true;
}

bool Controlador::aplicar_modificacion(const Modificacion& mod)
{
	if (mod.es_local) // Si es local tengo que comunicarle al servidor del cambio
	{
		sock1.enviar_flag(MODIFICACION);
		sock1.enviar_modif(mod);
		PacketID flag;
		sock1.recibir_flag(flag);
		if (flag == YA_APLICADA)
		{
			switch (mod.accion)
			{
			case NUEVO:
				base_de_datos.registrar_nuevo(mod.nombre_archivo);
				return true;
			case BORRADO:
				base_de_datos.registrar_eliminado(mod.nombre_archivo);
				return true;
			case EDITADO:
				base_de_datos.registrar_editado(mod.nombre_archivo);
				return true;
			case RENOMBRADO:
				base_de_datos.registrar_renombrado(mod.nombre_archivo, mod.nombre_alt_o_hash);
				return true;
			case COPIADO:
				base_de_datos.registrar_copiado(mod.nombre_archivo, mod.nombre_alt_o_hash);
				return true;
			default:
				return false;
			}
			return true;
		}
		if (flag == CONFLICTO)
		{
			syslog(LOG_ERR, "Hubo un conflicto al enviar la modificacion.");
			// Si hubo conflicto, probamos denuevo pero de otra manera
			Modificacion copia(mod);
			string nombreConflic(nombreConflictuado(mod.nombre_archivo));
			base_de_datos.renombrar(mod.nombre_archivo, nombreConflic);
			copia.nombre_archivo = nombreConflic;
			copia.accion = NUEVO;
			return aplicar_modificacion(copia);
		}
		if (flag != OK) return false; // Algo fallo?
	}
	switch (mod.accion)
	{
		case NUEVO:
			if (mod.es_local) { return enviar_nuevo_archivo(mod.nombre_archivo); }
			else { return pedir_nuevo_archivo(mod.nombre_archivo); }
		case EDITADO:
			if (mod.es_local) return enviar_edicion(mod);
			else return pedir_edicion(mod.nombre_archivo);
		case BORRADO:
			if (mod.es_local) return enviar_borrado(mod.nombre_archivo);
			else return borrar_archivo(mod.nombre_archivo);
		case RENOMBRADO:
			if (mod.es_local) return enviar_renombrado(mod.nombre_archivo, mod.nombre_alt_o_hash);
			else return renombrar_archivo(mod.nombre_archivo, mod.nombre_alt_o_hash);
		case COPIADO:
			if (mod.es_local) return enviar_copiado(mod.nombre_archivo, mod.nombre_alt_o_hash);
			else return copiar_archivo(mod.nombre_archivo, mod.nombre_alt_o_hash);
		default: // En realidad no existen otros casos salvo que funcione mal el soft o la red
			syslog(LOG_EMERG, "Modificacion desconocida!!");
			return true;
	}
	return false;
}

list<Modificacion> Controlador::comprobar_cambios_locales(){
        return base_de_datos.comprobar_cambios_locales();
}

