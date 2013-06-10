#ifndef SERVER_THREAD_COMUNICACION_CLIENTE_H_
#define SERVER_THREAD_COMUNICACION_CLIENTE_H_
#include "common_controlador.h"
#include "common_thread.h"
#include "common_mutex.h"
#include "common_socket_prot.h"
#include "common_base_de_datos_con_mutex.h"
/**@class ServerCommunicator server_thread_communicator.h
 * @brief clase que se encargará de la comunicacion con el cliente */
class ServerCommunicator : public Controlador, public Thread
{
private:
	Mutex mutex;
	void ejecutar();
	void procesar_flag(PacketID flag);
	list<ServerCommunicator*> *vinculados;
	BaseDeDatosConMutex base_de_datos;
public:
	ServerCommunicator(const char* dir, int fd1, int fd2);

	/**
	 * @brief Le asigna al comunicador una lista de vinculados para comunicar cambios
	 * @param vinc Lista de comunicadores vinculados por un usuario en comun
	 * @pre Los comunicadores pertenecen al mismo usuario
	 */
	void setVinculados(list<ServerCommunicator*> *vinc);

	/**
	 * @brief Con una modificacion recibida de un cliente, se la "aplica" al servidor.
	 * Tambien se encarga de todas las comunicaciones necesarias de por medio.
	 * @param mod Modificacion recibida
	 */
	void actuar_segun_modif_recibida(Modificacion &mod);

	/**
	 * @brief Notifica a todos los comunicadores vinculados que notifiquen a sus clientes
	 * @param mod La modificacion en cuestion
	 */
	void notificar_todos(Modificacion &mod);

	/**
	 * @brief Metodo para enviar al cliente conectado a este comunicador una notificacion de modificacion
	 * @param mod La modificacion en cuestion
	 */
	void propagar_cambio(Modificacion &mod);

	bool enviar_modificaciones();
};



#endif /* SERVER_THREAD_COMUNICACION_CLIENTE_H_ */
