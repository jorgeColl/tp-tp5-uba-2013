#include "cliente_notificador.h"
#include "cliente_controlador.h"

ClienteNotificador::ClienteNotificador(ClienteControlador* aNotificar) : aNotificar(aNotificar) {}

void ClienteNotificador::ejecutar()
{
	bool socketConectado = true;
	while (correr && socketConectado)
	{
		Modificacion modif;
		socketConectado = socket.recibir_modif(modif);
		if (socketConectado) cout << "Recibida notificacion: " << modif << endl;
		aNotificar->AplicarNotificacion(modif);
	}
}

void ClienteNotificador::conectar(const char* server,const char* puerto)
{
	socket.conectar(server, puerto);
}
