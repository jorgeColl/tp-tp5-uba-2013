/*
 * cliente_controlador.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */
#include <string>
#include <iostream>
#include "cliente_controlador.h"
#include "client_socket.h"
#include "common_socket.h"
using namespace std;
ClienteControlador::ClienteControlador(string server, string puerto){

}
bool ClienteControlador::login(string usuario, string contrasenia){
	cout<<"clienteControlador: me hicieron login con:"<<endl;
	cout<<"usuario:"<<usuario<<" contreseña:"<<contrasenia<<endl;
	bool exito;
	exito = sock.abrir_conexion();
	if(!exito) {return false;}

	string mensaje;
	mensaje += "1 ";
	mensaje += usuario;
	mensaje += contrasenia;
	exito = sock.enviar_mensaje(mensaje);
	if(!exito) {return false;}

	exito = sock.recibir_mensaje(mensaje);
	if(!exito) {return false;}
	if(mensaje == "loggin accepted"){
		return true;
	}
	return false;
}
void ClienteControlador::set_directorio(std::string dir){
	this->dir = dir;
}

