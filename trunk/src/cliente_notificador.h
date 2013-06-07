#ifndef CLIENTENOTIFICADOR_H_
#define CLIENTENOTIFICADOR_H_

#include "common_thread.h"
#include "common_socket_prot.h"
#include "common_mutex.h"
#include <list>
using namespace std;

class ClienteControlador; // Forward declaration

class ClienteNotificador: public Thread
{
public:
	ClienteNotificador(ClienteControlador* aNotificar);
	/**
	 * @brief Conecta el socket interno de esta clase
	 * @param server Direccion del servidor
	 * @param puerto Puerto como string
	 */
	void conectar(const char* server,const char* puerto);
	SocketProt socket;
private:
	ClienteControlador* aNotificar;
	void ejecutar();
};

#endif /* CLIENTENOTIFICADOR_H_ */
