/*
 * common_modificacion.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_modificacion.h"

using namespace std;

Modificacion::Modificacion(tipo_accion accion, const string &nombre_archivo, const string &nombre_archivo_alt)
	: accion(accion), nombre_archivo(nombre_archivo), nombre_archivo_alt(nombre_archivo_alt) {}

/**@brief Imprime una modificacion como si fuera solo texto */
ostream& operator<<(ostream& os, Modificacion& mod) {
	// Coso asqueroso para que imprima en palabras y no en numeros
	string accion;
	switch (mod.accion) {
	case (SUBIR_NUEVO_ARCHIVO):
		accion = "SUBIR_NUEVO_ARCHIVO";
		break;
	case (BAJAR_NUEVO_ARCHIVO):
		accion = "BAJAR_NUEVO_ARCHIVO";
		break;
	case (BORRAR_ARCHIVO_LOCAL):
		accion = "BORRAR_ARCHIVO_LOCAL";
		break;
	case (MANDAR_A_BORRAR_ARCHIVO):
		accion = "MANDAR_A_BORRAR_ARCHIVO";
		break;
	case (SUBIR_MOD_ARCHIVO):
		accion = "SUBIR_MOD_ARCHIVO";
		break;
	case (BAJAR_MOD_ARCHIVO):
		accion = "BAJAR_MOD_ARCHIVO";
		break;
	case (MANDAR_RENOMBRE_ARCHIVO):
		accion = "MANDAR_RENOMBRE_ARCHIVO";
		break;
	case (RENOMBRAR_ARCHIVO_LOCAL):
		accion = "RENOMBRAR_ARCHIVO_LOCAL";
		break;
	case (MANDAR_COPIA_ARCHIVO):
		accion = "MANDAR_COPIA_ARCHIVO";
		break;
	case (COPIAR_ARCHIVO_LOCAL):
		accion = "COPIAR_ARCHIVO_LOCAL";
		break;
	}
	os << "Accion: "<< accion << endl;
	os << "Nombre de archivo: "<< mod.nombre_archivo << endl;
	if (mod.nombre_archivo_alt != "")
		os << "Nombre de archivo alternativo: "<< mod.nombre_archivo_alt << endl;
	return os ;
}

bool Modificacion::Modificacion::operator==(const Modificacion &otra)
{
	return (accion == otra.accion && nombre_archivo == otra.nombre_archivo
			&& nombre_archivo_alt == otra.nombre_archivo_alt);
}
