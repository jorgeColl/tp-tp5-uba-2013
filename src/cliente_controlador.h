/*
 * cliente.h
 *
 *  Created on: 19/05/2013
 *      Author: jorge
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_
#include <string>
#include <vector>
#include "common_modificacion.h"
#include "client_socket.h"
/**
 * @class Cliente cliente.h
 * @brief Clase que se encargará de las acciones generales que puede hace un cliente
 */
class ClienteControlador {
ClientSocket sock;
std::string dir;
public:
	ClienteControlador(std::string server, std::string puerto);
	/**
	 * @brief Intentará iniciar sesion con el usuario y contraseña ingresados
	 * @details Se comunicarà con el servidor y este le responderá si el usuario y contraseña son correctos
	 * @return True si el login fue exitoso, false en caso contrario
	 */
	bool login(std::string usuario,std::string contrasenia);

	/** @brief POSIBLE FUNCION A IMPLEMENTAR */
	void logout();

	/** @brief Setea el directorio en donde se guardan los archivos del usuario */
	void set_directorio(std::string dir);

	/**@brief Recorre los archivos en busca de modificaciones a cada modificacion
	 * encontrada se genera una nueva instancia de la clase Modificacion y se agrega
	 * al vector
	 */
	std::vector<Modificacion> polling();

	/**@brief envia las modificaciones encontradas en los propios archivos*/
	bool enviar_modificaciones(std::vector<Modificacion> modifcaciones);

	/**@brief pide lista de modificaciones al servidor */
	bool recibir_modificaciones();

	/**@brief ejecuta las modificaciones que pidió anteriormente*/
	bool ejecutar_modificaciones();

	/**@brief pide el archivo cuyo nombre es ingresado al server
	 * @details pide el archivo al servidor y lo guarda en su correspondiente lugar
	 *  utilizando a la clase BaseDeDatos
	 * @return devuelve true si toda la operacion fue un exito, caso contrario devuelve false */
	bool pedir_nuevo_archivo(std::string& nombre_archivo);

	/**@brief enviar el archivo cuyo nombre es ingresado
	 * @details el archivo a enviar será abierto y a medida que se lee se envia
	 * @return devuelve true si la operacion fue un exito, caso contrario devuelve false */
	bool enviar_archivo(std::string& nombre_archivo);

};

#endif /* CLIENTE_H_ */
