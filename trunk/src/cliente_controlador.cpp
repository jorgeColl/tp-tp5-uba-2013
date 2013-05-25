#include <string>
#include <iostream>
#include <sstream>
#include "cliente_controlador.h"

using namespace std;

bool ClienteControlador::login(string server, string puerto1, string puerto2, string usuario, string contrasenia)
{
	cout<<"clienteControlador: me hicieron login con:"<<endl;
	cout<<"usuario:"<<usuario<<" contreseña:"<<contrasenia<<endl;
	bool exito;
	//Estos tiran excepciones dependiendo que falle
	sock1.conectar(server.c_str(),puerto1.c_str());
	sock2.conectar(server.c_str(),puerto2.c_str());

	string mensaje;
	mensaje += "1 ";
	mensaje += usuario;
	mensaje += contrasenia;
	exito = sock1.enviarLen((void*)mensaje.c_str(), mensaje.size());
	if(!exito) {return false;}

	Packet login;
	exito = sock1.recibir_flag(login);
	if(!exito) {return false;}
	if(login == LOGIN_OK)
	{
		return true;
	}
	return false;
}

void ClienteControlador::set_directorio(std::string dir){
	this->dir = dir;
}

bool ClienteControlador::armar_indice_local() {
	return base_de_datos.abrir(dir);
}

vector<Modificacion> ClienteControlador::pedir_y_comparar_indices() {
	// pide a la base de datos que actualice el indice local
	base_de_datos.actualizar_indice();
	// mensaje al server pidiendo el indice suyo
	recibir_indice();
	// realiza la comparacion entre el indice local y el recibido por el server
	return base_de_datos.comparar_indices();
}

vector<Modificacion> ClienteControlador::recibir_modificaciones() {
	vector<Modificacion> aux;
	return aux;
}

bool ClienteControlador::borrar_archivo(std::string& nombre_archivo){
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

bool ClienteControlador::enviar_modificacion(Modificacion& mod){
	// le manda un mensaje al server indicando que un archivo se modificó y le pasa los datos
	return true;
}

bool ClienteControlador::pedir_modificacion(std::string& nombre_archivo){
	// falta definir
	return true;
}

bool ClienteControlador::aplicar_modificacion(Modificacion& mod)
{
	switch (mod.fuente)
	{
	case SERVER:
		switch (mod.tipo)
		{
		case NUEVO:
			return pedir_nuevo_archivo(mod.nombre_archivo);
		case MODIFICADO:
			return pedir_modificacion(mod.nombre_archivo);
		case BORRADO:
			return borrar_archivo(mod.nombre_archivo);
		}
		break;
	case CLIENTE:
		switch (mod.tipo)
		{
		case NUEVO:
			return enviar_nuevo_archivo(mod.nombre_archivo);
		case MODIFICADO:
			return enviar_modificacion(mod);
		case BORRADO:
			return mandar_a_borrar(mod.nombre_archivo);
		}
		break;
	}
	return false;
}

bool ClienteControlador::start() {
	bool exito = armar_indice_local();
	if(!exito){return false;}

	vector<Modificacion> mod1 = pedir_y_comparar_indices();
	for(size_t i=0; i<mod1.size(); ++i){
		exito = aplicar_modificacion(mod1[i]);
		//Todo: if (exito) basededatos.aplicarmodif o similar
	}
	while(terminar != true) {
		vector<Modificacion> mod2 = comprobar_cambios_locales();
		for(size_t i=0;i<mod2.size();++i){
			//mod2[i].efectuar_cambios(*this);
			exito = base_de_datos.aplicar_cambios_locales(mod2[i]);
			if(!exito){return false;}
			exito = enviar_modificacion(mod2[i]);
			if(!exito){return false;}
		}
		vector<Modificacion> mod3 = recibir_modificaciones();
		for(size_t i=0;i<mod3.size();++i){
			//mod3[i].efectuar_cambios(*this);
			exito = base_de_datos.aplicar_cambios_locales(mod3[i]);
			if(!exito){return false;}
		}

		sleep(3);
	}
	return true;

}
std::vector<Modificacion> ClienteControlador::comprobar_cambios_locales(){
	return base_de_datos.comprobar_cambios_locales();
}


