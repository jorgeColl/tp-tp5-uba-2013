/*
 * server_accepter.h
 *
 *  Created on: 28/05/2013
 *      Author: jorge
 */

#ifndef SERVER_ACCEPTER_H_
#define SERVER_ACCEPTER_H_
#include "common_base_de_datos_usuario.h"
#include "server_thread_communicator.h"
#include "common_socket_prot.h"
#include "common_thread.h"
#include "common_mutex.h"

#define MAX_COLA 100
#define CONEXCIONES_ACEPTADAS_PARA_BORRAR_MUERTOS 1

class Accepter: public Thread
{
private:
	BaseDeDatosUsuario base_datos_usu;
	string dir;
	string puerto1;
	string puerto2;
	map<string, list<ServerCommunicator*> > comunicadores;
	SocketProt sock_prot1;
	SocketProt sock_prot2;
	Mutex mutex;

	void limpiar_procesos_terminados();
	/**
	 * @brief Metodo a llamar internamente al correr el thread
	 */
	virtual void ejecutar();
public:

	/**
	 * @brief Constructor
	 * @param dir Directorio a usar
	 * @param puerto1 Puerto a usar para conexiones y paso de datos
	 * @param puerto2 Puerto a usar para notificaciones
	 */
	Accepter(const string &dir, const string &puerto1, const string &puerto2);

	/** @brief Destructor. Cierra los sockets si quedaron abierto. */
	~Accepter();

	/**
	 * @brief Le pide al thread y asociados gentilmente que se detengan
	 */
	virtual void stop();

	/**@brief acepta una conexion y crea threads de acuerdo al mensaje
	 * @details utiliza al socket para aceptar/rechazar conexiones entrantes
	 * y si es valido el usuario genera una instancia de la clase ServerComunicator
	 *  y lo ejecuta en otro thread*/
	bool aceptar_conexion();

};

#endif /* SERVER_ACCEPTER_H_ */
