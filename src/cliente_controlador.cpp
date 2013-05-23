/*
 * cliente_controlador.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */
#include <string>
#include <iostream>

#include "cliente_controlador.h"

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
vector<Modificacion> ClienteControlador::pedir_y_comparar_indices(){

	vector<Modificacion> aux;
	return aux;
}
vector<Modificacion> ClienteControlador::recibir_modificaciones(){
	vector<Modificacion> aux;
	return aux;
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
bool ClienteControlador::borrar_archvio(std::string& nombre_archivo){
	return base_de_datos.eliminar_archivo(nombre_archivo);
}
bool ClienteControlador::mandar_a_borrar (std::string& nombre_archivo){
	// envia mensaje a server de borrado de archivo con el nombre de este
	return true;
}
bool ClienteControlador::pedir_nuevo_archivo(std::string& nombre_archivo){
	// manda mensaje a server pidiendo que transmita archivo
	// le dice a base de datos que guarde al nuevo archivo
	return true;
}

bool ClienteControlador::enviar_nuevo_archivo(std::string& nombre_archivo){
	// asumo que el archivo entra en ram (en un string)
	// pide a base de datos el archivo en string
	// pasa el archivo (que está en forma de string) al server
	return true;
}
bool ClienteControlador::modificar_archivo(std::string& nombre_archivo){
	//falta definir
	return true;
}
bool ClienteControlador::mandar_modificacion(std::string& nombre_archivo){
	// le manda un mensaje al server indicando que un archivo se modificó y le pasa los datos
	return true;
}
bool ClienteControlador::pedir_modificacion(std::string& nombre_archivo){
	// falta definir
	return true;
}

