/*
 * common_modificacion.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_modificacion.h"

using namespace std;

Modificacion::Modificacion(tipo_accion accion, bool es_local, const string &nombre_archivo, const string &nombre_archivo_alt)
	: accion(accion), es_local(es_local), nombre_archivo(nombre_archivo), nombre_archivo_alt(nombre_archivo_alt) {}

/**@brief Imprime una modificacion como si fuera solo texto */
ostream& operator<<(ostream& os, Modificacion& mod) {
	// Coso asqueroso para que imprima en palabras y no en numeros
	string accion;
	switch (mod.accion)
	{
	case (NUEVO):
		accion = "NUEVO";
		break;
	case (BORRADO):
		accion = "BORRADO";
		break;
	case (MODIFICADO):
		accion = "MODIFICADO";
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
	if (mod.nombre_archivo_alt != "")
		os << "Nombre de archivo alternativo: "<< mod.nombre_archivo_alt << endl;
	return os ;
}

bool Modificacion::operator==(const Modificacion &otra)
{
	return (accion == otra.accion && nombre_archivo == otra.nombre_archivo
			&& nombre_archivo_alt == otra.nombre_archivo_alt);
}

bool Modificacion::operator<(const Modificacion &otra)
{
	return (nombre_archivo < otra.nombre_archivo);
}
