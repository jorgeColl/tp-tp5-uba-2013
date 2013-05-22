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
#include "common_modificacion.h"

using namespace std;
ClienteControlador::ClienteControlador(string server, string puerto){
	terminar = false;
	dir = "default";
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
bool ClienteControlador::armar_indice_local() {
	return base_de_datos.armar_indice_local();
}
void ClienteControlador::start() {
	bool exito = armar_indice_local();
	if(!exito){	return;}

	vector<Modificacion> mod1 = pedir_y_comparar_indices();
	for(size_t i=0;i<mod1.size();++i){
		mod1[i].efectuar_cambios(*this);
	}
	while(terminar != true) {
		vector<Modificacion> mod2 = comprobar_cambios_locales();
		for(size_t i=0;i<mod2.size();++i){
			mod2[i].efectuar_cambios(*this);
		}
		vector<Modificacion> mod3 = recibir_modificaciones();
		for(size_t i=0;i<mod3.size();++i){
			mod3[i].efectuar_cambios(*this);
		}

		sleep(3);
	}

}
std::vector<Modificacion> ClienteControlador::comprobar_cambios_locales(){
	return base_de_datos.comprobar_cambios_locales();
}
