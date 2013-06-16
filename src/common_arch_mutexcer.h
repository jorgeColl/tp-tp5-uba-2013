
#ifndef COMMON_ARCH_MUTEXCER_H_
#define COMMON_ARCH_MUTEXCER_H_
#include <map>
#include <vector>
#include "common_mutex.h"
#include <string>
#include <iostream>
class ArchMutexcer {
	std::string dir;
	std::map <std::string,Mutex*>* mutex_archivos;
	Mutex mutex_loc;
protected:
	/**@brief resta una "pseudinstancia" */
	static void restar_instancia(ArchMutexcer* mutexcer);
	/**@brief busca en el directorio todos los nombres de archivos y le asigna un mutex unico para cada uno */
	void construir_mutexs();
	/**@brief devuelve el mutex correspondiente al nombre de archivo ingresado */
	/**@brief devuelve la cantidad de "pseudoinstancias" que hay */
	static size_t cant_instacias(ArchMutexcer* mutexcer);
	void new_mutex(const char* dir_nuevo_archivo);
	/**@brief elimina el mutex correspondiente a un archivo */
public:
	static Mutex mutex_clase;
	static std::map <std::string,ArchMutexcer*> hijitos;
	static std::map <ArchMutexcer*,size_t> cant_hijitos;

	/**@brief no usar este, usar generar_archmutexcer para conseguir una instancia */
	ArchMutexcer(const char* dir);
	/**
	 * @brief construye un ArchMutexcer
	 * @details dado un directorio (LA CARPETA DEL USUARIO), primero se fija que el ArchMutexcer
	 * correspondiente a ese directorio no fuese creado antes, en caso
	 * de que ya hubiese sido creado devuelve el puntero a este , en
	 * caso contrario crea un ArchMutexcer para ese directorio ingresado
	 * y genera un mutex para cada uno de los archivos dentro de él
	 */
	static ArchMutexcer* generar_archmutexcer(const char* dir);

	Mutex* get_mutex (const char* dir_archivo);
	/**@brief si se genera un nuevo archivo, se tiene que ingresarlo al sistema con este metodo */

	void delete_mutex(const char* dir_archivo_a_eliminar);
	/**@brief devuelve true o false si el dir ingresado tiene un mutex asociado */
	bool is_mutex(const char* dir_archivo);
	/**@brief resta 1 al contador de instancias, si quedaba una instancia y se llama a
	 * borrar, este elimina de la memoria a la instancia
	 */
	void borrar();

	~ArchMutexcer();

	friend std::ostream& operator<<(std::ostream& os, ArchMutexcer& archm);
};

std::ostream& operator<<(std::ostream& os, ArchMutexcer& archm);

class SmartP  {
	ArchMutexcer* mu;
public:
	SmartP(const std::string& dir);
	SmartP(ArchMutexcer* mutx);
	ArchMutexcer& data ();
	~SmartP();
};

#endif /* COMMON_ARCH_MUTEXCER_H_ */
