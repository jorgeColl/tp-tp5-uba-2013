#ifndef COMMON_CONTROLADOR_H_
#define COMMON_CONTROLADOR_H_
#include <string>
#include <vector>
#include "common_base_de_datos.h"
#include "common_modificacion.h"
#include "common_socket_prot.h"

/**@class Controlador common_controlador.h
 * @brief Clase base para manejo de lógica tras comunicarse cliente y servidor
 */

class Controlador
{
protected:
	string dir;
	SocketProt sock1;
	SocketProt sock2;
	string password;
	BaseDeDatos base_de_datos;
public:
	bool conectado;

	Controlador();

	Controlador(const string &dir, int sockfd1, int sockfd2, const string &password);

	/** @brief Setea el directorio en donde se guardan los archivos del usuario
	 * OJO: usar antes del start()  */
	void set_directorio(std::string dir);

	/**@brief le pide a la base de datos que arme el indice local podria estar por defecto
	 * en el constructor de la base de datos pero es mas legible si lo llamo explicitamente
	 * @throws Excepcion si no logra armarse el indice local */
	void armar_indice_local();

	/**@brief pide lista de modificaciones al servidor
	 */
	std::list<Modificacion> recibir_modificaciones();

	/**@brief Recorre los archivos en busca de modificaciones a cada modificacion
	 * encontrada se genera una nueva instancia de la clase Modificacion y se agrega
	 * al vector
	 */
	std::list<Modificacion> comprobar_cambios_locales();

	/**@brief efectua una modificacion, segun los parametros de modificacion, la accion a realizar variarÃ¡
	 */
	bool aplicar_modificacion(const Modificacion& mod);

	/**
	 * @brief Pide a la base de datos local que borre el archivo y registre el borrado
	 */
	bool borrar_archivo(const string &nombre_archivo);

	/**
	 * @brief Pide a la base de datos local que copie el archivo y registre el copiado
	 */
	bool copiar_archivo(const string &nombre_archivo, const string &nombre_archivo_anterior);

	/**
	 * @brief pide a la base de datos local que renombre el archivo y registre el renombrado
	 */
	bool renombrar_archivo(const string &nombre_archivo, const string &nombre_archivo_anterior);

	/**@brief pide el archivo cuyo nombre es ingresado al otro extremo
	 * @details pide el archivo al otro extremo y lo guarda en su correspondiente lugar
	 *  utilizando a la clase BaseDeDatos
	 * @return devuelve true si toda la operacion fue un exito, caso contrario devuelve false
	 */
	bool pedir_nuevo_archivo(const string &nombre_archivo);

	/**@brief enviar el archivo cuyo nombre es ingresado
	 * @details el archivo a enviar sera abierto y a medida que se lee se envia
	 * @return devuelve true si la operacion fue un exito, caso contrario devuelve false
	 */
	bool enviar_nuevo_archivo(const string &nombre_archivo);
	/**
	 * @brief Recibe un nuevo archivo
	 * @return True si la operacion fue un exito, caso contrario false
	 */
	bool recibir_nuevo_archivo(const string &nombre_archivo);

	/**@brief envia un mensaje al server con los datos necesarios para realizar la
	 *  edicion del archivo ingresado
	 */
	bool enviar_edicion(const Modificacion &mod);

	/**@brief Pide al server la informacion necesaria para poder realizar la
	 *  edicion del archivo y lo edita
	 */
	bool pedir_edicion(const string &nombre_archivo);

	/** @brief Envia un mensaje de borrado de archivo, si tiene exito lo registra en el indice local
	 */
	bool enviar_borrado(const string& nombre_archivo);

	/** @brief Envia un mensaje de renombrado de archivo, si tiene exito lo registra en el indice local
	 */
	bool enviar_renombrado(const string& nombre_archivo_nuevo, const string& nombre_archivo_viejo);

	/** @brief Envia un mensaje de copiad de archivo, si tiene exito lo registra en el indice local
	 */
	bool enviar_copiado(const string& nombre_archivo_nuevo, const string& nombre_archivo_viejo);

	virtual ~Controlador() {}
};

#endif /* COMMON_CONTROLADOR_H_ */
