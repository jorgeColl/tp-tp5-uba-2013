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
/**
 * @class Cliente cliente.h
 * @brief Clase que se encargará de las acciones generales que puede hace un cliente
 */
class Cliente {

public:
	/**
	 * @brief Intentará iniciar sesion con el usuario y contraseña ingresados
	 * @details Se comunicarà con el servidor y este le responderá si el usuario y contraseña son correctos
	 * @return True si el login fue exitoso, false en caso contrario
	 */
	bool login(std::string usuario, std::string contrasenia);
	/** @brief POSIBLE FUNCION A IMPLEMENTAR */
	void logout();
	/** @brief Setea el directorio en donde se guardan los archivos del usuario */
	void set_directorio(const char* dir);
	std::vector<std::string> polling();
};



#endif /* CLIENTE_H_ */
