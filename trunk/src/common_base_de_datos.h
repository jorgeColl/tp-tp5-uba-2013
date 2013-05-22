/*
 * common_base_de_datos.h
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#ifndef COMMON_BASE_DE_DATOS_H_
#define COMMON_BASE_DE_DATOS_H_
#include "common_modificacion.h"
using namespace std;
class BaseDeDatos {
public:
	bool set_directorio(std::string dir);
	bool armar_indice_local();
	/**@brief se encarga de encontrar los archivos modificados y
	 *  generar un vector de modificaciones*/
	vector<Modificacion> comprobar_cambios_locales();
	bool agregar_archivo(std::string nombre_archivo, std::string datos);
	bool eliminar_archivo(std::string nombre_archivo);
	bool modificar_archvivo();
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
