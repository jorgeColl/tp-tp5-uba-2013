#include "cliente_notificador.h"
#include "cliente_controlador.h"
#include "defines.h"
#include <syslog.h>

ClienteNotificador::ClienteNotificador(ClienteControlador* aNotificar) : aNotificar(aNotificar) {}

void ClienteNotificador::ejecutar()
{
	try
	{
		bool socketConectado = true;
		correr = true;
		while (correr && socketConectado)
		{
			Modificacion modif;
			socket.recibir_modif(modif);
			syslog(LOG_DEBUG, "Se recibio una notificacion.");
			aNotificar->AplicarNotificacion(modif);
		}
	}
	catch (exception &e)
	{
		syslog(LOG_DEBUG, "Se perdio la conexion del socket 2.");
		aNotificar->stop();
	}
}

void ClienteNotificador::conectar(const char* server,const char* puerto)
{
	socket.conectar(server, puerto);
}
