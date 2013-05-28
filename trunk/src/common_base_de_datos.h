#ifndef COMMON_BASE_DE_DATOS_H_
#define COMMON_BASE_DE_DATOS_H_

#include <fstream>
#include <vector>
#include "common_modificacion.h"

using namespace std;

#define NOMBRE_ARCH_DEF ".auindice"
#define EXT_TMP ".tmp"

class BaseDeDatos
{
public:
	/**
	 * @brief Abre el archivo asociado si existe, y sino lo crea y luego lo abre.
	 * @param directorio Directorio donde se ubicara el archivo
	 * @return true si pudo accederse al archivo, false en otro caso
	 */
	bool abrir(const string &directorio);

	//----- Modificacion de archivos en el directorio

	/**
	 * @brief Abre un archivo y devuelve el ifstream correspondiente al archivo abierto
	 * @param nombre_archivo Nombre del archivo (solo el nombre, no el path)
	 * @param ifstream Aqui se intentara abrir el archivo
	 * @return True si pudo abrirse correctamente el archivo
	 * @post Si la operacion fue exitosa ifstream esta abierto para lectura en modo binario
	 */
	bool abrir_para_leer(const string &nombre_archivo, ifstream &ifstream);

	/**
	 * @brief Dado un nombre de archivo, crea y abre un nuevo archivo en el ofstream
	 * @param nombre_archivo Nombre del archivo (solo el nombre, no el path)
	 * @param ifstream Aqui se intentara abrir el archivo
	 * @return True si pudo crearse y abrirse correctamente el archivo
	 * @post Si la operacion fue exitosa ofstream esta abierto para escritura en modo binario
	 */
	bool abrir_para_escribir(const string& nombre_archivo, ofstream &ofstream);

	/**
	 * @brief Lo mismo que el metodo anterior pero le agrega como extension al archivo un .tmp
	 */
	bool abrir_para_escribir_temporal(const string& nombre_archivo, ofstream &ofstream);

	/**
	 * @brief si la carga de datos al archivo finalizó y se realizó con exito,
	 *  se guarda con el verdadero nombre y se lo indexa
	 */
	bool renombrar(const string &viejo_nombre, const string &nuevo_nombre);

	/**
	 * @brief Lo mismo que el metodo renombrar pero los archivos nombres de archivo son los mismos salvo extension
	 * @param nombre_archivo Nombre de archivo compartido entre el original y el temporal
	 * @return True si la operacion fue exitosa
	 */
	bool renombrar_temporal(const string &nombre_archivo);

	/**
	 * @brief Elimina un archivo del directorio. Si se lo quiere eliminar de la indexacion luego llamar a registrar_eliminado
	 */
	bool eliminar_archivo(const string &nombre_archivo);

	//----- Registracion en el indice de eventos

	/**
	 * @brief Registra un nuevo archivo en la indexacion
	 * @pre El archivo existe y esta en el directorio correcto y no fue previamente indexado
	 * @return True si la operacion tiene exito
	 */
	bool registrar_nuevo(const string &nombre_archivo);

	/**
	 * @brief Registra que se elimino un archivo de la indexacion
	 * @pre El archivo no exista y habia sido previamente indexado
	 * @return True si la operacion tiene exito
	 */
	bool registrar_eliminado(const string &nombre_archivo);

	/**
	 * @brief Registra que se cambiaron los contenidos de un archivo
	 * @pre El archivo existe y habia sido previamente indexado
	 * @return True si la operacion tiene exito
	 */
	bool registrar_modificado(const string &nombre_archivo);

	/**
	 * @brief Registra que se cambio el nombre de un archivo ya indexado
	 * @pre El archivo de nombre_viejo habia sido previamente indexado
	 * @return True si la operacion tiene exito
	 */
	bool registrar_renombrado(const string &nombre_viejo, const string &nombre_nuevo);

	//----- Operaciones pertinentes al indice y registro de cambios

	/**@brief se encarga de encontrar los archivos modificados y
	 *  generar un vector de modificaciones
	 */
	vector<Modificacion> comprobar_cambios_locales();

	/**
	 * @brief Compara el indice local con otro devuelve las discrepancias
	 *
	 */
	vector<Modificacion> comparar_indices(fstream &otro);

private:
	/**
	 * @brief Carga los contenidos del archivo indice a una estructura en ram
	 */
	void cargarARam();

	string directorio;
	string pathArchivo;
	fstream archivo;
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
