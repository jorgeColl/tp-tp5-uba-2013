#ifndef MODIFICACION_H_
#define MODIFICACION_H_

#include <string>
#include <iostream>
using namespace std;

enum tipo_accion{ SUBIR_NUEVO_ARCHIVO, BAJAR_NUEVO_ARCHIVO,
	BORRAR_ARCHIVO_LOCAL, MANDAR_A_BORRAR_ARCHIVO,
	SUBIR_MOD_ARCHIVO, BAJAR_MOD_ARCHIVO,
	MANDAR_RENOMBRE_ARCHIVO, RENOMBRAR_ARCHIVO_LOCAL,
	MANDAR_COPIA_ARCHIVO, COPIAR_ARCHIVO_LOCAL };

class Modificacion {

public:
	/**
	 * @brief Constructor por parametros explicitos
	 * @param accion Tipo de accion representado por esta modificacion
	 * @param nombre_archivo Nombre del archivo relacionado
	 * @param nombre_archivo_alt Nombre extra en caso de ser necesario, parametro opcional
	 */
	Modificacion(tipo_accion accion, const string &nombre_archivo, const string &nombre_archivo_alt = string());

	/**
	 * @brief Constructor por deserializacion
	 * @param tiraBytes String que contiene la modificacion en formato serializado
	 */
	Modificacion(string tiraBytes);

	/**@brief Serializa el objeto y devuelve una tira de bytes */
	string serializar();

	tipo_accion accion;
	string nombre_archivo;
	string nombre_archivo_alt;
};

ostream& operator<<(std::ostream& os, Modificacion& mod);

#endif /* MODIFICACION_H_ */
