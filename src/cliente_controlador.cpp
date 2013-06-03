#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include "cliente_controlador.h"
using namespace std;

ClienteControlador::ClienteControlador() : Controlador(),delay_polling(POLLING_DEFAULT) {}

void ClienteControlador::login(string server, string puerto1, string puerto2,
		string usuario, string contrasenia, string polling)
{
	try
	{
		cout << "Intentado conectarse" << endl;
		//Estos tiran excepciones dependiendo que falle
		sock1.conectar(server.c_str(), puerto1.c_str());
		sock1.enviar_flag(LOGIN);
		sock1.enviar_msg_c_prefijo(usuario, BYTES_USER_PASS);
		sock1.enviar_msg_c_prefijo(contrasenia, BYTES_USER_PASS);
		PacketID login;
		sock1.recibir_flag(login);
		if(login != OK) throw runtime_error("Los datos de login son incorrectos.");
		sock2.conectar(server.c_str(),puerto2.c_str());
	}
	catch(exception &e) //Cierro los sockets y propago
	{
		sock1.cerrar();
		sock2.cerrar();
		throw e;
	}
}

list<Modificacion> ClienteControlador::pedir_y_comparar_indices()
{
	sock1.enviar_flag(PEDIDO_INDICE);
	stringstream idxServ;
	sock1.recibir_archivo(idxServ);
	list<Modificacion> modifs = base_de_datos.comparar_indices(idxServ);
	return modifs;
}

list<Modificacion> ClienteControlador::recibir_modificaciones() {
	list<Modificacion> aux;
	return aux;
}

void ClienteControlador::start()
{
	//Aplicacion inicial
	bool exito;
	list<Modificacion> mod1 = pedir_y_comparar_indices();
	for (list<Modificacion>::iterator it = mod1.begin(); it != mod1.end(); ++it)
	{
		aplicar_modificacion(*it);
	}

	while(terminar != true)
	{
		sleep(delay_polling);
		list<Modificacion> mod2 = comprobar_cambios_locales();
		for (list<Modificacion>::iterator it = mod2.begin(); it != mod2.end(); ++it)
		{
			//mod2[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(*it);
			if(!exito) return;
		}
		list<Modificacion> mod3 = recibir_modificaciones();
		for (list<Modificacion>::iterator it = mod3.begin(); it != mod3.end(); ++it)
		{
			//mod3[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(*it);
			if(!exito) return;
		}
	}
	return;
}

