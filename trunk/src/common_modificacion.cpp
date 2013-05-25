/*
 * common_modificacion.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_modificacion.h"
using namespace std;

Modificacion::Modificacion(Fuente fuente, Tipo tipo, const std::string &nombre_archivo)
	: fuente(fuente),tipo(tipo),nombre_archivo(nombre_archivo) {}

