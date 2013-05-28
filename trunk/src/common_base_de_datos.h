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
	/**
	 * @brief abre un archivo y devuelve el ifstream correspondiente al archivo abierto
	 * @details sirve para que despues otras clases lean del archivo abierto
	 */
	std::ifstream* abrir_para_leer(std::string& nombre_archivo);
	/**
	 * @brief dado un nombre de archivo ,crea y abre un nuevo archivo y devuelve el ofstream
	 *  correspondiente a ese archivo
	 *  @details esto sirve para que otras clases llenen con datos este archivo
	 */
	std::ofstream* generar_archivo_temp(std::string& nombre_archivo);
	/**
	 * @brief si la carga de datos al archivo finalizó y se realizó con exito,
	 *  se guarda con el verdadero nombre y se lo indexa
	 */
	bool cerrar_exitosamente_archivo(std::string nombre_archivo, std::ofstream& fd);
	/**@brief se encarga de encontrar los archivos modificados y
	 *  generar un vector de modificaciones
	 */
	std::vector<Modificacion> comprobar_cambios_locales();
	/**@brief compara dos indices, uno que es el recibido por el servidor y el otro que es el local
	 * y genera un vector de modificaciones
	 */
	std::vector<Modificacion> comparar_indices();

	/**@brief agrega un archivo nuevo
	 * @details modifica indice para que tenga al nuevo archivo , y le calcula su hash/hashes
	 */
	// POR AHORA LO SACO
	//bool agregar_archivo(std::string nombre_archivo, std::string datos);

	/**@brief elimina un archivo del directorio
	 * @details elimina los datos del indice con de ese archivo eliminado
	 */
	bool eliminar_archivo(std::string nombre_archivo);

	// FALTA DEFINIR FORMA DE QUE PARAMETROS PASAR !!!
	bool modificar_archvivo();

private:
	void cargarARam();
	string directorio;
	string pathArchivo;
	fstream archivo;
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
