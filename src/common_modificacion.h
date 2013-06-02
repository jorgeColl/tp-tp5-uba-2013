#ifndef MODIFICACION_H_
#define MODIFICACION_H_

#include <string>
#include <iostream>
using namespace std;

enum tipo_accion{ NUEVO, BORRADO, MODIFICADO, RENOMBRADO, COPIADO };

class Modificacion {

public:
	/**
	 * @brief Constructor por parametros explicitos
	 * @param accion Tipo de accion representado por esta modificacion
	 * @param nombre_archivo Nombre del archivo relacionado
	 * @param nombre_archivo_alt Nombre extra en caso de ser necesario, parametro opcional
	 */
	Modificacion(tipo_accion accion, bool es_local, const string &nombre_archivo, const string &nombre_archivo_alt = string());

	/**
	 * @brief Constructor por deserializacion
	 * @param tiraBytes String que contiene la modificacion en formato serializado
	 */
	Modificacion(string tiraBytes, bool es_local);

	/**@brief Serializa el objeto y devuelve una tira de bytes */
	string serializar();

	bool operator==(const Modificacion &otra);

	tipo_accion accion;
	bool es_local;
	string nombre_archivo;
	string nombre_archivo_alt;
};

ostream& operator<<(std::ostream& os, Modificacion& mod);

#endif /* MODIFICACION_H_ */
