/*
 * modificacion.h
 *
 *  Created on: 20/05/2013
 *      Author: jorge
 */

#ifndef MODIFICACION_H_
#define MODIFICACION_H_

#include <string>

enum Fuente {SERVER, CLIENTE};
enum Tipo{ NUEVO, BORRADO, MODIFICADO };

class Modificacion {

public:
	/**
	 * @brief Constructor por deserializacion
	 * @param cadenaBytes String que contiene los byes a deserializar
	 */
	Modificacion(std::string &cadenaBytes);
	/**
	 * @brief Crea una modificacion
	 * @param fuente Quien "creo" esta modificacion.
	 * @param tipo Si el cambio es un arch nuevo, uno modificado, o uno borrado
	 * @param nombre_archivo Nombre del archivo
	 */
	Modificacion(Fuente fuente, Tipo tipo, const std::string &nombre_archivo);
	/**
	 * @brief Convierte la informacion del objeto en una cadena de bytes
	 * @return String con la cadena de bytes
	 */
	std::string serializar();
	/**
	 * @brief Interpreta al string recibido modifica los de este objeto
	 * @post Este objeto cambio sus atributos
	 */
	void desserializar(std::string &cadenaBytes);
	Fuente fuente;
	Tipo tipo;
	std::string nombre_archivo;
};

#endif /* MODIFICACION_H_ */
