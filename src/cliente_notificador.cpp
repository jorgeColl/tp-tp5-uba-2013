#include "cliente_notificador.h"

bool ClienteNotificador::hay_modificaciones()
{
	return !modifs.empty();
}

Modificacion ClienteNotificador::pedir_modificacion()
{
	Lock lock(mutexLista);
	Modificacion mod = modifs.front();
	modifs.pop_front();
	return mod;
}

void ClienteNotificador::ejecutar()
{
	bool socketConectado = true;
	while (correr && socketConectado)
	{
		Modificacion modif;
		socketConectado = socket.recibir_modif(modif);
		if (socketConectado) cout << "Recibida notificacion: " << modif << endl;
		Lock lock(mutexLista);
		modifs.push_back(modif);
	}
}

void ClienteNotificador::conectar(const char* server,const char* puerto)
{
	socket.conectar(server, puerto);
}
