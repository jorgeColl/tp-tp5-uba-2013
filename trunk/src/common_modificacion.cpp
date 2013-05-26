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

