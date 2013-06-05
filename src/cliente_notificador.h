#ifndef CLIENTENOTIFICADOR_H_
#define CLIENTENOTIFICADOR_H_

#include "common_thread.h"
#include "common_socket_prot.h"
#include "common_mutex.h"
#include <list>
using namespace std;

class ClienteNotificador: public Thread
{
public:
	/**
	 * @brief Conecta el socket interno de esta clase
	 * @param server Direccion del servidor
	 * @param puerto Puerto como string
	 */
	void conectar(const char* server,const char* puerto);
	/**
	 * @brief Se fija si hay modificaciones
	 * @return True si hay modificaciones
	 */
	bool hay_modificaciones();
	/**
	 * @brief Devuelve una modificacion, si es que hay
	 * @return La modificacion
	 * @post La modificacion devuelta fue quitada de la lista
	 */
	Modificacion pedir_modificacion();
	Mutex mutexLista;
	SocketProt socket;
private:
	void ejecutar();
	list<Modificacion> modifs;
};

#endif /* CLIENTENOTIFICADOR_H_ */
