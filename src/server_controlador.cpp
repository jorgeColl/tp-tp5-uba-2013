/*
 * server_controlador.cpp
 *
 *  Created on: 24/05/2013
 *      Author: jorge
 */
#include "server_controlador.h"
#include <sstream>
ServerControlador::ServerControlador(std::string puerto, std::string directorio) {
	this->puerto=0;
	stringstream ss;
	ss<<puerto;
	ss>>this->puerto;
	this->dir=directorio;
}


void ServerControlador::start(){
	return;
}
