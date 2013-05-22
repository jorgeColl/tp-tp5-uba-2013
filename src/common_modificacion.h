/*
 * modificacion.h
 *
 *  Created on: 20/05/2013
 *      Author: jorge
 */

#ifndef MODIFICACION_H_
#define MODIFICACION_H_
#include <string>
class ClienteControlador;

class Modificacion {
public:
	/**@brief metodo que convierte a la instancia en un string para luego ser
	 * pasada a desserializar y volver a ser una instancia pero del ordenador
	 * en donde llegó el mensaje
	 */
	std::string serializar();
	/**@brief interpreta al string recibido modifica los atributos internos de
	 * acuerdo al string recibido
	 */
	void desserializar(std::string);

	/**@brief despues de modificar sus atributos internos con desserializar,
	 *  este metodo hace efectivo esa modificacion del otro lado */
	bool efectuar_cambios(ClienteControlador& cliente);
};


#endif /* MODIFICACION_H_ */
