/*
 * codificador.h
 *
 *  Created on: 19/05/2013
 *      Author: jorge
 */

#ifndef CODIFICADOR_H_
#define CODIFICADOR_H_
#include <string>
/**
 * @class Codificador common_codificador.h
 * @brief Clase que se encargará de codificar un mensaje previo a mandarlo
 * asi como a decodificar un mensaje recibido
 */
class Codificador {

public:
	/** @brief codifica un mensaje usando la contraseña del usuario */
	bool codificar(std::string& mensaje);
	/** @brief decodifica un mensaje usando la contraseña del usuario */
	bool decodificar(std::string& mensaje);
};



#endif /* CODIFICADOR_H_ */
