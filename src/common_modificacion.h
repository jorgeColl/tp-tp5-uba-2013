/*
 * modificacion.h
 *
 *  Created on: 20/05/2013
 *      Author: jorge
 */

#ifndef MODIFICACION_H_
#define MODIFICACION_H_

#include <string>

enum tipo_accion{ SUBIR_NUEVO_ARCHIVO, BAJAR_NUEVO_ARCHIVO, BORRAR_ARCHIVO_LOCAL, MANDAR_A_BORRAR_ARCHIVO, SUBIR_MOD_ARCHIVO, BAJAR_MOD_ARCHIVO };

class Modificacion {

public:
	tipo_accion accion;
	std::string nombre_archivo;
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
};

#endif /* MODIFICACION_H_ */
