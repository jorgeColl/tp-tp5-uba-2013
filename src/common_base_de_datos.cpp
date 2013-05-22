/*
 * common_base_de_datos.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_base_de_datos.h"

using namespace std;
bool BaseDeDatos::set_directorio(std::string dir) {
	return true;
}
bool BaseDeDatos::armar_indice_local() {
	return true;
}
/**@brief se encarga de encontrar los archivos modificados y
 *  generar un vector de modificaciones*/
vector<Modificacion> BaseDeDatos::comprobar_cambios_locales() {
	std::vector<Modificacion> aux;
	return aux;
}
bool BaseDeDatos::agregar_archivo(std::string nombre_archivo,
		std::string datos) {
	return true;
}
bool BaseDeDatos::eliminar_archivo(std::string nombre_archivo) {
	return true;
}
bool BaseDeDatos::modificar_archvivo(){
	return true;
}
