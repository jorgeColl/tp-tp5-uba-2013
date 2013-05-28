#ifndef COMMON_BASE_DE_DATOS_H_
#define COMMON_BASE_DE_DATOS_H_

#include <fstream>
#include <vector>
#include <stdint.h> 		//uint8_t
#include <linux/limits.h>	//NAME_MAX
#include <list>
#include "common_modificacion.h"

using namespace std;

#define NOMBRE_ARCH_DEF ".auindice"
#define EXT_TMP ".tmp"
#define BYTES_HASH 16
//Definicion de bytes para prefijo de largo de nombre de archivo
#define BYTES_PREF_NOMBRE 1
#if NAME_MAX > 255 //Si el sistema operativo permite nombres mas largos
#undef BYTES_PREF_NOMBRE
#define BYTES_PREF_NOMBRE 2
#endif


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
	bool abrir_para_leer(const string &nombre_archivo, fstream &ifstream);

	/**
	 * @brief Dado un nombre de archivo, crea y abre un nuevo archivo en el ofstream
	 * @param nombre_archivo Nombre del archivo (solo el nombre, no el path)
	 * @param ifstream Aqui se intentara abrir el archivo
	 * @return True si pudo crearse y abrirse correctamente el archivo
	 * @post Si la operacion fue exitosa ofstream esta abierto para escritura en modo binario
	 */
	bool abrir_para_escribir(const string& nombre_archivo, fstream &ofstream);

	/**
	 * @brief Lo mismo que el metodo anterior pero le agrega como extension al archivo un .tmp
	 */
	bool abrir_para_escribir_temporal(const string& nombre_archivo, fstream &ofstream);

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
	 * @class RegistroIndice
	 * @brief Clase auxiliar para pasar datos a ram
	 */
	class RegistroIndice
	{
		public:
			/** @brief Constructor por toma de datos de archivo fisico */
			RegistroIndice(const string &nombre_archivo, const string &dir);
			/** @brief Constructor por parametros explicitos */
			RegistroIndice(const string &nombre, time_t modif, off_t tam, const string &hash);
			/** @brief Constructor por deserializacion */
			RegistroIndice(const char* bytes, uint8_t tamNombre);
			/** @brief Devuelve el registro serializado */
			string serializar();
			/** @brief Calcula el hash del archivo y lo agrega al reg en el campo corresp
			 * @return True si fue exitoso */
			bool calcularHash();
			/** @brief Devuelve el tam maximo que puede ocupar un registro */
			static size_t tamMax();
			/** @brief Devuelve el tam que ocupa un registro dado su prefijo de longitud */
			static size_t tamReg(size_t prefijo);
			string nombre;
			time_t modif;
			off_t tam;
			string hash;
	};

	/**
	 * @class IndiceRam
	 * @brief Clase auxiliar para contener en ram el indice
	 */
	class IndiceRam
	{
		public:
			void cargar(fstream &arch);
			void agregar(RegistroIndice &reg);
			RegistroIndice* buscarNombre(string &nombre);
			RegistroIndice* buscarFecha(time_t fecha);
			list<RegistroIndice*> buscarTam(off_t tam);
			RegistroIndice* buscarHash(string &hash);
		private:
			list<RegistroIndice> almacenamiento;
	};

	//----- Modificacion del archivo fisico

	/**
	 * @brief Persiste un nuevo archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	bool registrar_nuevo_fis(const RegistroIndice &reg);

	/**
	 * @brief Persiste una eliminacion de archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	bool registrar_eliminado(const RegistroIndice &reg);

	/**
	 * @brief Persiste una modificacion de archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	bool registrar_modificado(const RegistroIndice &reg);

	/**
	 * @brief Persiste un renombramiento de archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	bool registrar_renombrado(const RegistroIndice &reg, const string &nombre_nuevo);

	/**
	 * @brief Carga los contenidos del archivo indice a una estructura en ram
	 */
	void cargarARam();

	string directorio;
	string pathArchivo;
	fstream archivo;
	IndiceRam indice;
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
