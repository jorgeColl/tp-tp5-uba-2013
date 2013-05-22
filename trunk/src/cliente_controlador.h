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
#include "client_socket.h"
#include "common_base_de_datos.h"
#include "common_modificacion.h"
/**
 * @class Cliente cliente.h
 * @brief Clase que se encargará de las acciones generales que puede hace un cliente
 */
class ClienteControlador {
ClientSocket sock;
std::string dir;
BaseDeDatos base_de_datos;
bool terminar;
private:
	std::vector<Modificacion> pedir_y_comparar_indices();
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

	/**@brief inicia la ejecucion del cliente */
	void start();

	/**@brief le pide a la base de datos que arme el indice local podria estar por defecto
	 * en el constructor de la base de datos pero es mas legible si lo llamo explicitamente*/
	bool armar_indice_local();

	/**@brief Recorre los archivos en busca de modificaciones a cada modificacion
	 * encontrada se genera una nueva instancia de la clase Modificacion y se agrega
	 * al vector
	 */
	std::vector<Modificacion> comprobar_cambios_locales();

	/**@brief pide lista de modificaciones al servidor */
	std::vector<Modificacion> recibir_modificaciones();

	/**@brief pide el archivo cuyo nombre es ingresado al server
	 * @details pide el archivo al servidor y lo guarda en su correspondiente lugar
	 *  utilizando a la clase BaseDeDatos
	 * @return devuelve true si toda la operacion fue un exito, caso contrario devuelve false */
	bool pedir_nuevo_archivo(std::string& nombre_archivo);

	/**@brief enviar el archivo cuyo nombre es ingresado
	 * @details el archivo a enviar será abierto y a medida que se lee se envia
	 * @return devuelve true si la operacion fue un exito, caso contrario devuelve false */
	bool enviar_nuevo_archivo (std::string& nombre_archivo);
	/**@brief pide a la base de datos que borre un archivo */
	bool borrar_archvio (std::string& nombre_archivo);
	/**@brief pide a la base de datos que modifique a un archvio */
	bool modificar_archivo (std::string& nombre_archivo);

};

#endif /* CLIENTE_H_ */
