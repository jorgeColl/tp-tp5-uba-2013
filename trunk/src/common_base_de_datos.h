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
#if NAME_MAX > 255 //Si el sistema operativo permite nombres mas largos le doy 2 bytes
#undef BYTES_PREF_NOMBRE
#define BYTES_PREF_NOMBRE 2
#endif

// TODO: Separar en mas clases?

/**
 * @class BaseDeDatos common_base_de_datos.h "common_base_de_datos.h"
 * @brief Clase que maneja el indexamiento de archivos y parte del manejo de los mismos
 */
class BaseDeDatos
{
public:
	virtual ~BaseDeDatos();

	/**
	 * @brief Abre el archivo asociado si existe, y sino lo crea y luego lo abre.
	 * @param dir Directorio donde se ubicara el archivo
	 * @return true si pudo accederse al archivo, false en otro caso
	 * @throws Excepcion si no pudo abrirse el documento
	 */
	void abrir(const string &dir);

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
	 * @param ofstream Aqui se intentara abrir el archivo
	 * @return True si pudo crearse y abrirse correctamente el archivo
	 * @post Si la operacion fue exitosa ofstream esta abierto para escritura en modo binario
	 */
	bool abrir_para_escribir(const string& nombre_archivo, ofstream &ofstream);

	/**
	 * @brief Lo mismo que el metodo anterior pero le agrega como extension al archivo un .tmp
	 */
	bool abrir_para_escribir_temporal(const string& nombre_archivo, ofstream &ofstream);

	/**
	 * @brief Renombra un archivo
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

	/**
	 * @brief Copia un archivo
	 */
	bool copiar(const string &viejo_nombre, const string &nuevo_nombre);

	//----- Registracion en el indice de eventos

	/**
	 * @brief Registra un nuevo archivo en la indexacion
	 * @pre El archivo existe y esta en el directorio correcto y no fue previamente indexado
	 * @throws Excepcion si falla
	 */
	void registrar_nuevo(const string &nombre_archivo);

	/**
	 * @brief Registra que se elimino un archivo de la indexacion
	 * @pre El archivo no exista y habia sido previamente indexado
	 * @throws Excepcion si falla
	 */
	void registrar_eliminado(const string &nombre_archivo);

	/**
	 * @brief Registra que se cambiaron los contenidos de un archivo
	 * @pre El archivo existe y habia sido previamente indexado
	 * @throws Excepcion si falla
	 */
	void registrar_modificado(const string &nombre_archivo);

	/**
	 * @brief Registra que se cambio el nombre de un archivo ya indexado
	 * @pre El archivo de nombre_viejo habia sido previamente indexado
	 * @throws Excepcion si falla
	 */
	void registrar_renombrado(const string &nombre_nuevo, const string &nombre_viejo);

	/**
	 * @brief Registra que se copio un archivo ya indexado
	 * @pre El archivo de nombre_viejo habia sido previamente indexado
	 * @throws Excepcion si falla
	 */
	void registrar_copiado(const string &nombre_nuevo, const string &nombre_viejo);

	//----- Operaciones pertinentes al indice y registro de cambios

	/**
	 * @brief Compara la informacion en el indice con los archivos fisicos y devuelve una lista de modificaciones
	 * @return Lista con las modificaciones a realizar para el cliente
	 */
	list<Modificacion> comprobar_cambios_locales();

	/**
	 * @brief Compara el indice local con otro y devuelve las discrepancias
	 * @return Lista con las modificaciones a realizar tras resolver los conflictos
	 */
	list<Modificacion> comparar_indices(iostream &otro);

	/**
	 * @brief Fusiona 2 listas de modificaciones de forma que queden resueltos los conflictos
	 * @pre La lista externa tiene modificaciones no locales, y la local modificaciones locales
	 * @param lista_externa Lista con los modificaciones recibidas de otro ladi
	 * @param lista_local Lista con las modificaciones halladas localmente con el indice local
	 * @return Lista con las modificaciones tras resolver los conflictos
	 * @post La lista_externa y la lista_local fueron ordenadas
	 */
	list<Modificacion> merge_modifs(list<Modificacion> &lista_externa, list<Modificacion> &lista_local);

	/**
	 * @param modif_externa Modificacion no local
	 * @param modif_local Modificacion local
	 * @return Modificaciones resultantes de resolver el conflicto
	 */
	list<Modificacion> resolver_conflicto(const Modificacion &modif_externa, const Modificacion &modif_local);


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
			RegistroIndice(const char* bytes, uint8_t tamNombre, uint32_t archOffset);
			/** @brief Devuelve el registro serializado */
			string serializar() const;
			/** @brief Calcula el hash del archivo y lo agrega al reg en el campo corresp
			 * @return True si fue exitoso */
			bool calcularHash(const string &dir, const string &password, string &hash);
			/** @brief Devuelve el tam maximo que puede ocupar un registro */
			static size_t tamMax();
			/** @brief Devuelve el tam que ocupa un registro dado su prefijo de longitud */
			static size_t tamReg(size_t prefijo);
			/** @brief overloads */
			bool operator==(const RegistroIndice &r2);
			string nombre;
			time_t modif;
			off_t tam;
			string hash;
			uint32_t archOffset;
	};

	/**
	 * @class IndiceRam
	 * @brief Clase auxiliar para contener en ram el indice
	 */
	class IndiceRam
	{
		public:
			/** @brief Carga el indice desde un archivo abiert */
			void cargar(istream &arch);
			/** @brief Agrega un registro al indice en ram */
			void agregar(RegistroIndice &reg);
			/** @brief Eliminar un registro al indice en ram */
			void eliminar(RegistroIndice &reg);
			/** @brief Modifica un registro del indice en ram */
			void modificar(RegistroIndice &reg, const string &password, const string &dir);
			/** @brief Renombra un registro al indice en ram */
			void renombrar(RegistroIndice &reg, const string &nombre_nuevo);
			/** @brief Devuelve un puntero al registro de un dado nombre
			 * @return Puntero al registro, o null si no se encontro */
			RegistroIndice* buscarNombre(const string &nombre);
			/** @brief Devuelve un puntero al registro de una dada fecha de modificado
			 * @return Puntero a lista de punteros a registros. Puede estar vacia si no se encontraron */
			list<RegistroIndice*> buscarFecha(const time_t fecha);
			/** @brief Devuelve un puntero a registros de un dado tamanio
			 * @return Puntero a lista de punteros a registros. Puede estar vacia si no se encontraron */
			list<RegistroIndice*> buscarTam(const off_t tam);
			/** @brief Devuelve un puntero a registros dado un hash
			 * @return Puntero al registro, o null si no se encontro */
			RegistroIndice* buscarHash(const string &hash);
			/** @brief Devuelve el nombre de todos los registros */
			list<string> devolverNombres();
		private:
			list<RegistroIndice> almacenamiento;
	};

	//----- Modificacion del archivo fisico

	/**
	 * @brief Persiste un nuevo archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	void registrar_nuevo_fis(RegistroIndice &reg);

	/**
	 * @brief Persiste una eliminacion de archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	void registrar_eliminado_fis(const RegistroIndice &reg);

	/**
	 * @brief Persiste una modificacion de archivo en la indexacion fisica
	 * @return True si la operacion tiene exito
	 */
	void registrar_modificado_fis(const RegistroIndice &reg);

	/**
	 * @brief Carga los contenidos del archivo indice a una estructura en ram
	 */
	void cargarARam();

	/**
	 * @brief Compara la informacion en el indice con los archivos fisicos y devuelve una lista de modificaciones
	 * @param indice Indice en ram a usar
	 * @param es_local Dice si las modificaciones a devolver seran locales o no
	 * @return Lista con las modificaciones a realizar
	 */
	list<Modificacion> comprobar_cambios(IndiceRam &indice, bool es_local);

	string password;
	string directorio;
	string pathArchivo;
	fstream archivo;
	IndiceRam indice;
};



#endif /* COMMON_BASE_DE_DATOS_H_ */
