#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>	// Excepciones estandar
#include "cliente_controlador.h"
using namespace std;

ClienteControlador::ClienteControlador() : Controlador(),delay_polling(POLLING_DEFAULT) {}

void ClienteControlador::login(string server, string puerto1, string puerto2,
		string usuario, string contrasenia, string polling)
{
	if (conectado) throw runtime_error("El cliente ya esta corriendo."); // Nunca deberia darse
	try
	{
		bool exito;
		base_de_datos.abrir(dir);
		cout << "Intentado conectarse" << endl;
		//Estos tiran excepciones dependiendo que falle
		sock1.conectar(server.c_str(), puerto1.c_str());
		exito = sock1.enviar_flag(LOGIN);
		if (!exito) throw runtime_error("Error al enviar el flag.");
		exito = sock1.enviar_msg_c_prefijo(usuario, BYTES_USER_PASS);
		if (!exito) throw runtime_error("Error al enviar el usuario.");
		exito = sock1.enviar_msg_c_prefijo(contrasenia, BYTES_USER_PASS);
		if (!exito) throw runtime_error("Error al enviar la password.");
		PacketID login;
		exito = sock1.recibir_flag(login);
		cout << "Recibio flag: " << login << endl;
		if(login != OK) throw runtime_error("Los datos de login son incorrectos.");
		sock2.conectar(server.c_str(),puerto2.c_str());
		conectado = true;
	}
	catch(runtime_error &e) //Cierro los sockets y propago
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

void ClienteControlador::ejecutar()
{
	bool exito;
	//Comentado temporalmente para testear hasta que este terminado
	/*
	cout << "Ejecutando sincronizacion inicial" << endl;
	// Comprobacion inicial
	list<Modificacion> mod1 = pedir_y_comparar_indices();
	for (list<Modificacion>::iterator it = mod1.begin(); it != mod1.end(); ++it)
	{
		aplicar_modificacion(*it);
	}
	*/
	while(correr && conectado)
	{
		cout << "Esperando los " << delay_polling << " segundos de polling." << endl;
		sleep(delay_polling);
		cout << "Ejecutando polling" << endl;
		list<Modificacion> mod2 = comprobar_cambios_locales();
		cout << "Numero de cambios locales encontrados: " << mod2.size() << endl;
		for (list<Modificacion>::iterator it = mod2.begin(); it != mod2.end(); ++it)
		{
			cout << *it << endl;
			exito = aplicar_modificacion(*it);
			if(!exito) throw runtime_error("Error al aplicar una modificacion local.");
		}
		// TODO: Poner esto en otro thread como corresponde
		list<Modificacion> mod3 = recibir_modificaciones();
		for (list<Modificacion>::iterator it = mod3.begin(); it != mod3.end(); ++it)
		{
			exito = aplicar_modificacion(*it);
			if(!exito) throw runtime_error("Error al aplicar una modificacion recibida.");
		}
	}
	return;
}

