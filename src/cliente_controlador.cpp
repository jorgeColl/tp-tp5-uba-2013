#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>	// Excepciones estandar
#include "cliente_controlador.h"
#include "common_hashing.h"
#include <syslog.h>
using namespace std;

ClienteControlador::ClienteControlador() : Controlador(),delay_polling(POLLING_DEFAULT),
		notificador(this) {}

void ClienteControlador::login(string server, string puerto1, string puerto2,
		string usuario, string contrasenia, string polling)
{
	if (conectado) throw runtime_error("El cliente ya esta corriendo."); // Nunca deberia darse
	try
	{
		base_de_datos.cerrar();
		sock1.cerrar();
		sock2.cerrar();
		delay_polling = atoi(polling.c_str());
		base_de_datos.abrir(dir);
		syslog(LOG_DEBUG, "Intentado conectarse.");
		//Estos tiran excepciones dependiendo que falle
		sock1.conectar(server.c_str(), puerto1.c_str());
		sock1.enviar_flag(LOGIN);
		sock1.enviar_msg_c_prefijo(usuario, BYTES_USER_PASS);
		//sock1.enviar_msg_c_prefijo(contrasenia, BYTES_USER_PASS);
		sock1.enviarLen(MD5_string(usuario+contrasenia).c_str(),BYTES_HASH);
		PacketID login;
		sock1.recibir_flag(login);
		if(login != OK) throw runtime_error("Los datos de login son incorrectos.");
		notificador.conectar(server.c_str(),puerto2.c_str());
		notificador.socket.set_password(contrasenia);
		conectado = true;
		sock1.set_password(contrasenia);
		sock2.set_password(contrasenia);
		syslog(LOG_INFO, "Conexion exitosa.");
	}
	catch(runtime_error &e) //Cierro los sockets y propago
	{
		base_de_datos.cerrar();
		sock1.cerrar();
		sock2.cerrar();
		throw e;
	}
}

list<Modificacion> ClienteControlador::pedir_y_comparar_indices()
{
	sock1.enviar_flag(PEDIDO_INDICE);
	// Lo recibo en ram pues lo voy a desechar tras comparar y es chico
	stringstream idxServ;
	sock1.recibir_archivo(idxServ);
	list<Modificacion> modifs = base_de_datos.comparar_indices(idxServ);
	return modifs;
}

void ClienteControlador::AplicarNotificacion(Modificacion &modif)
{
	Lock aplicandoCambio(mutexCambios);
	syslog(LOG_INFO, "Se recibio modificacion: %s", modif.toString().c_str());
	bool exito = aplicar_modificacion(modif);
	// Si falla, solo logeo, se supone que el indice fisico y demas quedo en condiciones
	if (!exito) syslog(LOG_ERR, "Error al aplicar la modificacion recibida.");
}

void ClienteControlador::ejecutar()
{
	try
		{
		bool exito;
		correr = true;
		//Comentado temporalmente para testear hasta que este terminado
		syslog(LOG_DEBUG, "Ejecutando sincronizacion inicial.");
		// Comprobacion inicial
		list<Modificacion> modServer = pedir_y_comparar_indices();
		for (list<Modificacion>::iterator it = modServer.begin(); it != modServer.end(); ++it)
		{
			syslog(LOG_INFO, "Modificacion inicial: %s", it->toString().c_str());
			aplicar_modificacion(*it);
		}
		notificador.start(); // Recien aca pongo a correr el notificador, no deberia haber "perdida de datos"
		while(correr && conectado)
		{
			syslog(LOG_DEBUG, "Esperando los %i segundos de polling.", delay_polling);
			sleep(delay_polling);
			Lock cambiosLocales(mutexCambios);
			syslog(LOG_DEBUG, "Ejecutando polling");
			list<Modificacion> modLocales = comprobar_cambios_locales();
			syslog(LOG_INFO, "Numero de cambios locales encontrados: %i", modLocales.size());
			for (list<Modificacion>::iterator it = modLocales.begin(); it != modLocales.end(); ++it)
			{
				syslog(LOG_INFO, "Modificacion local: %s", it->toString().c_str());
				exito = aplicar_modificacion(*it);
				// Si falla, solo logeo, se supone que el indice fisico y demas quedo en condiciones
				if (!exito) syslog(LOG_ERR, "Error al aplicar una modificacion local.");
			}
		}
		sock1.enviar_flag(LOGOUT);
	}
	catch(exception &e)
	{
		syslog(LOG_ERR, "Conexion perdida con el servidor");
		// TODO: Mostrar por la interface que se perdio la conexion. glib::slot no sirve con threads
	}
	sock1.cerrar();
	notificador.stop();
	notificador.join();
	conectado = false;
	return;
}

