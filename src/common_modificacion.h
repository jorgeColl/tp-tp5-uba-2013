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


enum tipo_accion{ SUBIR_NUEVO_ARCHIVO, BAJAR_NUEVO_ARCHIVO, BORRAR_ARCHIVO_LOCAL, MANDAR_A_BORRAR_ARCHIVO, SUBIR_MOD_ARCHIVO, BAJAR_MOD_ARCHIVO };

class Modificacion {
	tipo_accion accion;
	std::string nombre_archivo;
public:
	Modificacion(tipo_accion accion,std::string nombre_archivo);
	/**@brief metodo que convierte a la instancia en un string para luego ser
	 * pasada a desserializar y volver a ser una instancia pero del ordenador
	 * en donde llegó el mensaje
	 */
	std::string serializar();
	/**@brief interpreta al string recibido modifica los atributos internos de
	 * acuerdo al string recibido
	 */
	void desserializar(std::string);

	/**@brief este metodo hace efectivo la modificacion
	 * @details las posible acciones incluyen dos zonas distintas del programa,
	 * por un lado tenemos las acciones que puede ocurrir cuando el cliente
	 * sufre un cambio local en sus archivos y lo tiene que notificar al server
	 * y otro es cuando se pone a efectuar los cambios que el server le dijo
	 * que hiciera */
	bool efectuar_cambios(ClienteControlador& cliente);
};

#include "cliente_controlador.h"
#endif /* MODIFICACION_H_ */
