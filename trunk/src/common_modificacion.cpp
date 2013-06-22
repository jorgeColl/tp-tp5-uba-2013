/*
 * common_modificacion.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_modificacion.h"

using namespace std;

Modificacion::Modificacion() : accion(A_ZERO), es_local(false), nombre_archivo(), nombre_alt_o_hash() {}

Modificacion::Modificacion(tipo_accion accion, bool es_local, const string &nombre_archivo, const string &nombre_alt_o_hash)
	: accion(accion), es_local(es_local), nombre_archivo(nombre_archivo), nombre_alt_o_hash(nombre_alt_o_hash) {}

/**@brief Imprime una modificacion como si fuera solo texto */
ostream& operator<<(ostream& os, Modificacion& mod) {
	// Coso asqueroso para que imprima en palabras y no en numeros
	string accion;
	switch (mod.accion)
	{
	case (A_ZERO):
		accion = "VALOR NO ASIGNADO";
		break;
	case (NUEVO):
		accion = "NUEVO";
		break;
	case (BORRADO):
		accion = "BORRADO";
		break;
	case (EDITADO):
		accion = "EDITADO";
		break;
	case (COPIADO):
		accion = "COPIADO";
		break;
	case (RENOMBRADO):
		accion = "RENOMBRADO";
		break;
	}
	if (mod.es_local) os << "LOCAL" << endl;
	else os << "NO LOCAL" << endl;
	os << "Accion: "<< accion << endl;
	os << "Nombre de archivo: "<< mod.nombre_archivo << endl;
	if (mod.nombre_alt_o_hash != "")
		os << "Nombre de archivo alternativo/hash: "<< mod.nombre_alt_o_hash << endl;
	return os ;
}

bool Modificacion::operator==(const Modificacion &otra)
{
	return (accion == otra.accion && nombre_archivo == otra.nombre_archivo
			&& nombre_alt_o_hash == otra.nombre_alt_o_hash);
}

bool Modificacion::operator<(const Modificacion &otra)
{
	return (nombre_archivo < otra.nombre_archivo);
}
