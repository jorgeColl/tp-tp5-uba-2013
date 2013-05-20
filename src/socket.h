/*
 * socket.h
 *
 *  Created on: 19/05/2013
 *      Author: jorge
 */

#ifndef SOCKET_H_
#define SOCKET_H_
#include <string>
#include <cstdlib>
/**
 * @class Socket socket.h
 * @brief clase encargada de el envio y recepcion de datos a traves de un socket
 */
class Socket {

public:
	/** @brief recibe un mensaje */
	std::string recibir_mensaje();

	/** @brief envia un mensaje */
	bool enviar_mensaje();
	/** @brief intenta abrir un socket, devuelve true si lo logra en
	 * caso contrario devuevle false */
	bool abrir_conexion();

	/** @brief cierra el socket, devuelve true si no hubo inconvenientes,
	 * false si hubo algun problema */
	bool cerrar_conexion();

};


#endif /* SOCKET_H_ */
