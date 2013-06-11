/*
 * common_base_de_datos_con_mutex.h
 *
 *  Created on: 09/06/2013
 *      Author: jorge
 */

#ifndef COMMON_BASE_DE_DATOS_CON_MUTEX_H_
#define COMMON_BASE_DE_DATOS_CON_MUTEX_H_
#include "common_base_de_datos.h"
#include "common_arch_mutexcer.h"
#include "common_util.h"
class BaseDeDatosConMutex {
	BaseDeDatos base_de_datos;
	SmartP smpt;
	std::string dir;
	Mutex mutx_indice;
	map<string, Mutex*> mutexs_en_actividad;
public:
	// TODAS estos metodos solo llaman al mutex del archivo antes de llamar a la
	// verdadera base de datos
	BaseDeDatosConMutex(const string &dir);

	void abrir(const string &dir);

	bool abrir_para_leer(const string &nombre_archivo, ifstream &ifstream);

	bool abrir_para_escribir(const string& nombre_archivo, ofstream &ofstream);

	bool cerrar_archivo(string& nombre_archivo, ofstream& fd);

	bool cerrar_archivo(string& nombre_archivo, ifstream& fd);

	bool renombrar(const string &viejo_nombre, const string &nuevo_nombre);

	bool eliminar_archivo(const string &nombre_archivo);

	bool copiar(const string &viejo_nombre, const string &nuevo_nombre);

	void registrar_nuevo(const string &nombre_archivo);

	//void registrar_eliminado(const string &nombre_archivo);

	//void registrar_modificado(const string &nombre_archivo);

};

#endif /* COMMON_BASE_DE_DATOS_CON_MUTEX_H_ */
