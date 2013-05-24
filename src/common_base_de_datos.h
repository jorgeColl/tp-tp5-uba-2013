#ifndef COMMON_BASE_DE_DATOS_H_
#define COMMON_BASE_DE_DATOS_H_

#include <fstream>
#include <vector>
#include "common_modificacion.h"

using namespace std;

#define NOMBRE_ARCH_DEF ".auindice"

class BaseDeDatos
{
public:
	/**
	 * @brief Abre el archivo asociado si existe, y sino lo crea y luego lo abre.
	 * @param directorio Directorio donde se ubicara el archivo
	 * @return true si pudo accederse al archivo, false en otro caso
	 */
	bool abrir(const std::string &directorio);
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
private:
	void cargarARam();
	string directorio;
	string pathArchivo;
	fstream archivo;
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
