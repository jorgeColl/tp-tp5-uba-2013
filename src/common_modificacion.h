#ifndef MODIFICACION_H_
#define MODIFICACION_H_

#include <string>
#include <iostream>
using namespace std;

enum tipo_accion{ A_ZERO = 0, NUEVO, BORRADO, EDITADO, RENOMBRADO, COPIADO };

class Modificacion {

public:
	Modificacion();
	/**
	 * @brief Constructor por parametros explicitos
	 * @param accion Tipo de accion representado por esta modificacion
	 * @param es_local Si la modificacion fue o no generada en base al indice local
	 * @param nombre_archivo Nombre del archivo relacionado
	 * @param nombre_alt_o_hash Nombre extra en caso de ser necesario, parametro opcional
	 */
	Modificacion(tipo_accion accion, bool es_local, const string &nombre_archivo, const string &nombre_alt_o_hash = string());

	/**@brief Devuelve igualdad comparando accion, y ambos nombres de archivo*/
	bool operator==(const Modificacion &otra);

	/**@brief Devuelve desigualdad comparando solo el nombre de archivo*/
	bool operator<(const Modificacion &otra);

	tipo_accion accion;
	bool es_local;
	string nombre_archivo;
	string nombre_alt_o_hash;
};

ostream& operator<<(std::ostream& os, Modificacion& mod);

#endif /* MODIFICACION_H_ */
