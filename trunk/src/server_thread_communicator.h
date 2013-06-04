#ifndef SERVER_THREAD_COMUNICACION_CLIENTE_H_
#define SERVER_THREAD_COMUNICACION_CLIENTE_H_
#include "common_controlador.h"
#include "common_thread.h"
#include "common_mutex.h"
#include "common_socket_prot.h"

/**@class ServerCommunicator server_thread_communicator.h
 * @brief clase que se encargará de la comunicacion con el cliente */
class ServerCommunicator : public Controlador, public Thread
{
private:
	Mutex mutex;
	void ejecutar();
public:
	ServerCommunicator(const char* dir, int fd1, int fd2);

	bool enviar_modificaciones();
};



#endif /* SERVER_THREAD_COMUNICACION_CLIENTE_H_ */
