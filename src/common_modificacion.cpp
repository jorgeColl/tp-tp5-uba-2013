/*
 * common_modificacion.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_modificacion.h"

using namespace std;

Modificacion::Modificacion(tipo_accion accion,string nombre_archivo,size_t bloque_ini, size_t bloque_fin)
	: accion(accion),nombre_archivo(nombre_archivo) {
	this->bloque_ini = bloque_ini;
	this->bloque_fin = bloque_fin;
}

/**@brief muestra el estado interno del string */
ostream& operator<<(ostream& os, Modificacion& mod) {
	// coso asqueroso para que imprima en palabras y no en numeros
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
	}
	os << "Accion: "<<accion <<" Nombre de archivo: "<<mod.nombre_archivo<<endl;
	os <<"Bloque inicial: "<<mod.bloque_ini<<" Bloque final: "<<mod.bloque_fin;
	return os ;
}
