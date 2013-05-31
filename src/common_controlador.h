#ifndef COMMON_CONTROLADOR_H_
#define COMMON_CONTROLADOR_H_
#include <string>
#include <vector>
#include "common_base_de_datos.h"
#include "common_modificacion.h"
#include "common_socket_prot.h"
class Controlador {
protected:
	SocketProt sock1;
	SocketProt sock2;
	string dir;
	BaseDeDatos base_de_datos;
	bool terminar;
public:

	/** @brief Setea el directorio en donde se guardan los archivos del usuario
	 * OJO: usar antes del start()  */
	void set_directorio(std::string dir);

	/**@brief le pide a la base de datos que arme el indice local podria estar por defecto
	 * en el constructor de la base de datos pero es mas legible si lo llamo explicitamente
	 * @throws Excepcion si no logra armarse el indice local */
	void armar_indice_local();

	/**@brief pide lista de modificaciones al servidor
	 */
	std::vector<Modificacion> recibir_modificaciones();
	/**@brief Recorre los archivos en busca de modificaciones a cada modificacion
	 * encontrada se genera una nueva instancia de la clase Modificacion y se agrega
	 * al vector
	 */
	std::vector<Modificacion> comprobar_cambios_locales();
	/**@brief efectua una modificacion, segun los parametros de modificacion, la accion a realizar variarÃ¡
	 */
	bool aplicar_modificacion(Modificacion& mod);

	/** @brief pide a la base de datos que borre el archivo
	 */
	bool borrar_archivo(std::string& nombre_archivo);
	/**@brief envia un mensaje al otro extremo diciendo que se borre el archivo
	 */
	bool mandar_a_borrar(Modificacion& mod);
	/**@brief pide el archivo cuyo nombre es ingresado al otro extremo
	 * @details pide el archivo al otro extremo y lo guarda en su correspondiente lugar
	 *  utilizando a la clase BaseDeDatos
	 * @return devuelve true si toda la operacion fue un exito, caso contrario devuelve false
	 */
	bool pedir_nuevo_archivo(Modificacion& mod);
	/**@brief enviar el archivo cuyo nombre es ingresado
	 * @details el archivo a enviar serÃ¡ abierto y a medida que se lee se envia
	 * @return devuelve true si la operacion fue un exito, caso contrario devuelve false
	 */
	bool enviar_nuevo_archivo(std::string& nombre_archivo);

	/**@brief pide a la base de datos que modifique a un archvio
	 */
	bool modificar_archivo(std::string& nombre_archivo);

	/**@brief envia un mensaje al server con los datos necesarios para realizar la
	 *  modificacion del archivo ingresado
	 */
	bool enviar_modificacion(Modificacion& mod);

	/**@brief pide al server la informacion necesaria para poder realizar la
	 *  modificacion del archivo y lo modifica
	 */
	bool pedir_modificacion(std::string& nombre_archivo);

	//virtual ~Controlador() {}
};

#endif /* COMMON_CONTROLADOR_H_ */
