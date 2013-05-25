#ifndef CLIENTE_H_
#define CLIENTE_H_
#include <string>
#include <vector>
#include "common_socket_prot.h"
#include "common_base_de_datos.h"
#include "common_modificacion.h"

using namespace std;

/**
 * @class Cliente cliente_controlador.h
 * @brief Clase que se encargará de las acciones generales que puede hace un cliente
 */
class ClienteControlador
{
private:
	SocketProt sock1;
	SocketProt sock2;
	string dir;
	BaseDeDatos base_de_datos;
	bool terminar;
	vector<Modificacion> pedir_y_comparar_indices();
	bool recibir_indice();
public:
	ClienteControlador();
	/**
	 * @brief Intentará iniciar sesion con el usuario y contraseña ingresados, en el servidor y los puertos indicados
	 * @details Se comunicarà con el servidor y este le responderá si el usuario y contraseña son correctos
	 * @return True si el login fue exitoso, false en caso contrario
	 */
	bool login(string server, string puerto1, string puerto2, string usuario, string contrasenia);

	void logout();

	/** @brief Setea el directorio en donde se guardan los archivos del usuario
	 * OJO: usar antes del start()  */
	void set_directorio(string dir);

	/**@brief inicia la ejecucion del cliente */
	bool start();

	/**@brief le pide a la base de datos que arme el indice local podria estar por defecto
	 * en el constructor de la base de datos pero es mas legible si lo llamo explicitamente*/
	bool armar_indice_local();

	/**@brief Recorre los archivos en busca de modificaciones a cada modificacion
	 * encontrada se genera una nueva instancia de la clase Modificacion y se agrega
	 * al vector
	 */
	std::vector<Modificacion> comprobar_cambios_locales();
	bool aplicar_modificacion(Modificacion& mod);
	/**@brief pide lista de modificaciones al servidor */
	std::vector<Modificacion> recibir_modificaciones();

	/** @brief pide a la base de datos que borre el archivo*/
	bool borrar_archivo(std::string& nombre_archivo);
	/**@brief envia un mensaje al server diciendo que se borre el archivo */
	bool mandar_a_borrar (std::string& nombre_archivo);
	/**@brief pide el archivo cuyo nombre es ingresado al server
	 * @details pide el archivo al servidor y lo guarda en su correspondiente lugar
	 *  utilizando a la clase BaseDeDatos
	 * @return devuelve true si toda la operacion fue un exito, caso contrario devuelve false */
	bool pedir_nuevo_archivo(std::string& nombre_archivo);
	/**@brief enviar el archivo cuyo nombre es ingresado
	 * @details el archivo a enviar será abierto y a medida que se lee se envia
	 * @return devuelve true si la operacion fue un exito, caso contrario devuelve false */
	bool enviar_nuevo_archivo(std::string& nombre_archivo);

	/**@brief pide a la base de datos que modifique a un archvio */
	bool modificar_archivo(std::string& nombre_archivo);

	/**@brief envia un mensaje al server con los datos necesarios para realizar la
	 *  modificacion del archivo ingresado*/
	bool enviar_modificacion(Modificacion& mod);

	/**@brief pide al server la informacion necesaria para poder realizar la
	 *  modificacion del archivo y lo modifica*/
	bool pedir_modificacion(std::string& nombre_archivo);

};

#endif /* CLIENTE_H_ */
