/*
 * common_base_de_datos.h
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#ifndef COMMON_BASE_DE_DATOS_H_
#define COMMON_BASE_DE_DATOS_H_
#include <vector>
#include "common_modificacion.h"

class BaseDeDatos {
public:
	bool set_directorio(std::string dir);
	bool armar_indice_local();
	/**@brief se encarga de encontrar los archivos modificados y
	 *  generar un vector de modificaciones*/
	std::vector<Modificacion> comprobar_cambios_locales();
	/**@brief agrega un archivo nuevo
	 * @details modifica indice para que tenga al nuevo archivo , y le calcula su hash/hashes
	 */
	bool agregar_archivo(std::string nombre_archivo, std::string datos);
	/**@brief elimina un archivo del directorio
	 * @details elimina los datos del indice con de ese archivo eliminado
	 */
	bool eliminar_archivo(std::string nombre_archivo);
	bool modificar_archvivo();
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
