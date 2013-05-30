/*
 * common_arch_mutexcer.h
 *
 *  Created on: 29/05/2013
 *      Author: jorge
 */

#ifndef COMMON_ARCH_MUTEXCER_H_
#define COMMON_ARCH_MUTEXCER_H_
#include <map>
#include <vector>
#include "common_mutex.h"

class ArchMutexcer {
	const char* dir;
public:
	static std::vector<ArchMutexcer*> a_eliminar;
	static std::map <const char*,ArchMutexcer*> hijitos;
	static std::map <ArchMutexcer*,size_t> cant_hijitos;
	std::map <const char*,Mutex*> mutex_archivos;

	ArchMutexcer(const char* dir);
	/**
	 * @brief construye un ArchMutexcer
	 * @details dado un directorio, primero se fija que el ArchMutexcer
	 * correspondiente a ese directorio no fuese creado antes, en caso
	 * de que ya hubiese sido creado devuelve el puntero a este , en
	 * caso contrario crea un ArchMutexcer para ese directorio ingresado
	 * y genera un mutex para cada uno de los archivos dentro de él
	 */
	static ArchMutexcer* generar_archmutexcer(const char* dir);
	/**@brief devuelve la cantidad de "pseudoinstancias" que hay */
	static size_t cant_instacias(ArchMutexcer* mutexcer);
	/**@brief resta una "pseudinstancia" */
	static void restar_instancia(ArchMutexcer* mutexcer);
	/**@brief busca en el directorio todos los nombres de archivos y le asigna un mutex unico para cada uno */
	void construir_mutexs();
	/**@brief devuelve el mutex correspondiente al nombre de archivo ingresado */
	Mutex* get_mutex (const char* dir_archivo);
	/**@brief si se genera un nuevo archivo, se tiene que ingresarlo al sistema con este metodo */
	void new_mutex(const char* dir_nuevo_archivo);

	~ArchMutexcer();
};



#endif /* COMMON_ARCH_MUTEXCER_H_ */
