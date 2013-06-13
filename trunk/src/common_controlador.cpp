#include "common_controlador.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>	// Memcpy
#include "common_util.h"
#include "common_hashing.h"

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

bool Controlador::pedir_nuevo_archivo(string& nombre_archivo){
	// Manda mensaje a server pidiendo que transmita archivo
	// Le dice a base de datos que guarde al nuevo archivo
	sock1.enviar_flag(PEDIDO_ARCHIVO_ENTERO);
	sock1.enviar_msg_c_prefijo(nombre_archivo, 1);
	recibir_nuevo_archivo(nombre_archivo);
	base_de_datos.registrar_nuevo(nombre_archivo);
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

bool Controlador::enviar_edicion(Modificacion& mod)
{
	ifstream archivo;
	base_de_datos.abrir_para_leer(mod.nombre_archivo, archivo);
	archivo.seekg(0, ios::end);
	off_t tam = archivo.tellg();
	archivo.seekg(0);
	off_t bloques = tam / TAM_BLOQ;
	if (tam % TAM_BLOQ != 0) ++bloques;
	sock1.enviarLen((char*)&bloques, sizeof(off_t)); // Mando la cantidad de bloques del archivo
	for (off_t i = 0; i < bloques; ++i) // Mando todos los hashes por bloques del archivo
	{
		string hash;
		MD5_bloque(archivo, password, i*TAM_BLOQ, TAM_BLOQ, hash);
		sock1.enviarLen(hash.c_str(), BYTES_HASH);
	}
	// Espero que el servidor me mande la lista de bloques que necesita
	list<off_t> bloqMandar;
	size_t numBloqMandar = 0;
	sock1.recibirLen((char*)&numBloqMandar, sizeof(size_t));
	for (size_t i = 0; i < numBloqMandar; ++i)
	{
		off_t numBloq = 0;
		sock1.recibirLen((char*)&numBloq, sizeof(off_t));
		bloqMandar.push_back(numBloq);
	}
	// Envio esos bloques, en orden
	for(list<off_t>::iterator it = bloqMandar.begin(); it != bloqMandar.end(); ++it)
	{
		sock1.enviar_pedazo_archivo(archivo, *it*TAM_BLOQ, TAM_BLOQ);
	}
	archivo.close();

	PacketID flag;
	sock1.recibir_flag(flag);
	if (flag != OK) return false;
	base_de_datos.registrar_modificado(mod.nombre_archivo);
	return true;
}

bool Controlador::pedir_edicion(std::string& nombre_archivo)
{

	sock1.enviar_flag(PEDIDO_HASHES_BLOQUES);
	// Mando el flag
	// Recibo la cantida de bloques
	// Comparo
	// Creo un archivo temporario
	// falta definir
	return true;
}

bool Controlador::aplicar_modificacion(Modificacion& mod)
{
	if (mod.es_local) // Si es local tengo que comunicarle al servidor del cambio
	{
		cout<<"LOCAL"<<endl;
		cout<<"enviando flag MODIFICACION "<<endl;
		sock1.enviar_flag(MODIFICACION);
		cout<<"enviando modificacion"<<endl;
		sock1.enviar_modif(mod);
		PacketID flag;
		cout<<"recibiendo flag"<<endl;
		sock1.recibir_flag(flag);
		cout<<"flag recibido"<<endl;
		if (flag == YA_APLICADA) {
			switch (mod.accion)
			{
			case NUEVO:
				base_de_datos.registrar_nuevo(mod.nombre_archivo);
				return true;
			case BORRADO:
				base_de_datos.registrar_eliminado(mod.nombre_archivo);
				return true;
			default:
				return false;
			}
			return true;
		}
		if (flag != OK) return false; // Algo fallo?
	}
	switch (mod.accion)
	{
		case NUEVO:
			cout<<"NUEVO"<<endl;
			if (mod.es_local) { return enviar_nuevo_archivo(mod.nombre_archivo); }
			else { return pedir_nuevo_archivo(mod.nombre_archivo); }
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
			cout<<"ALERT !!!! modificacion desconocida!!"<<endl;
			return true;
	}
	return false;
}

list<Modificacion> Controlador::comprobar_cambios_locales(){
        return base_de_datos.comprobar_cambios_locales();
}

