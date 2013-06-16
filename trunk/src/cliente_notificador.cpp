#include "cliente_notificador.h"
#include "cliente_controlador.h"

ClienteNotificador::ClienteNotificador(ClienteControlador* aNotificar) : aNotificar(aNotificar) {}

void ClienteNotificador::ejecutar()
{
	bool socketConectado = true;
	correr = true;
	while (correr && socketConectado)
	{
		Modificacion modif;
		socket.recibir_modif(modif);
		cout << "Recibida notificacion." << endl;
		aNotificar->AplicarNotificacion(modif);
	}
}

void ClienteNotificador::conectar(const char* server,const char* puerto)
{
	socket.conectar(server, puerto);
}
