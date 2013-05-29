#ifndef SERVER_THREAD_COMUNICACION_CLIENTE_H_
#define SERVER_THREAD_COMUNICACION_CLIENTE_H_
#include "common_controlador.h"
/**@class ServerCommunicator server_thread_communicator.h
 * @brief clase que se encargará de la comunicacion con el cliente */
class ServerCommunicator : public Controlador {
public:
	ServerCommunicator(const char* dir, int fd1,int fd2);

	void start();

	bool enviar_modificaciones();
};



#endif /* SERVER_THREAD_COMUNICACION_CLIENTE_H_ */
