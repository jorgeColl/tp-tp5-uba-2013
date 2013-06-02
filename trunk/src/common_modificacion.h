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
	 * @param es_local Si la modificacion fue o no generada en base al indice local
	 * @param nombre_archivo Nombre del archivo relacionado
	 * @param nombre_archivo_alt Nombre extra en caso de ser necesario, parametro opcional
	 */
	Modificacion(tipo_accion accion, bool es_local, const string &nombre_archivo, const string &nombre_archivo_alt = string());

	/**
	 * @brief Constructor por deserializacion
	 * @param es_local Si la modificacion fue o no generada en base al indice local
	 * @param tiraBytes String que contiene la modificacion en formato serializado
	 */
	Modificacion(string tiraBytes, bool es_local);

	/**@brief Serializa el objeto y devuelve una tira de bytes*/
	string serializar();

	/**@brief Devuelve igualdad comparando accion, y ambos nombres de archivo*/
	bool operator==(const Modificacion &otra);

	/**@brief Devuelve desigualdad comparando solo el nombre de archivo*/
	bool operator<(const Modificacion &otra);

	tipo_accion accion;
	bool es_local;
	string nombre_archivo;
	string nombre_archivo_alt;
};

ostream& operator<<(std::ostream& os, Modificacion& mod);

#endif /* MODIFICACION_H_ */
