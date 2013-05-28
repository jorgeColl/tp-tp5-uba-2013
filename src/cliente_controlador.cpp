#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "cliente_controlador.h"

using namespace std;
ClienteControlador::ClienteControlador(){
	terminar = false;
	dir = "default";

}
bool ClienteControlador::login(std::string server, std::string puerto1, std::string puerto2, std::string usuario, std::string contrasenia)
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
	exito = sock1.enviarLen(mensaje.c_str(), mensaje.size());
	if(!exito) {return false;}

	Packet login;
	exito = sock1.recibir_flag(login);
	if(!exito) {return false;}
	if(login == OK)
	{
		return true;
	}
	return false;
}

vector<Modificacion> ClienteControlador::pedir_y_comparar_indices() {
	// mensaje al server pidiendo el indice suyo
	//recibir_indice();
	// realiza la comparacion entre el indice local y el recibido por el server
	//return base_de_datos.comparar_indices();
	return vector<Modificacion>();
}
vector<Modificacion> ClienteControlador::recibir_modificaciones() {
	vector<Modificacion> aux;
	return aux;
}

void ClienteControlador::start() {
	bool exito = armar_indice_local();
	if(!exito) return;

	vector<Modificacion> mod1 = pedir_y_comparar_indices();
	for(size_t i=0; i<mod1.size(); ++i){
		//mod1[i].efectuar_cambios(*this);
		exito = aplicar_modificacion(mod1[i]);
	}
	while(terminar != true) {
		vector<Modificacion> mod2 = comprobar_cambios_locales();
		for(size_t i=0;i<mod2.size();++i){
			//mod2[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(mod2[i]);
			if(!exito) return;
		}
		vector<Modificacion> mod3 = recibir_modificaciones();
		for(size_t i=0;i<mod3.size();++i){
			//mod3[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(mod3[i]);
			if(!exito) return;
		}

		sleep(3);
	}
	return;
}

