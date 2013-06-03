/*
 * server_accepter.h
 *
 *  Created on: 28/05/2013
 *      Author: jorge
 */

#ifndef SERVER_ACCEPTER_H_
#define SERVER_ACCEPTER_H_
#include "server_base_de_datos_usuario.h"
#include "server_thread_communicator.h"
#include "common_socket_prot.h"
#include "common_thread.h"
#include "common_mutex.h"

#define MAX_COLA 100
// usa puerto 1
class Accepter: public Thread {
	bool correr;
	BaseDeDatosUsuario base_datos_usu;
	const char* dir;
	vector<ServerCommunicator*> comunicadores;
	string puerto1;
	string puerto2;
	SocketProt sock_prot1;
	SocketProt sock_prot2;
	Mutex mutex;
public:
	Accepter(const char* dir, const char* puerto1, const char* puerto2) :
		correr(false), base_datos_usu(dir), dir(dir), puerto1(puerto1), puerto2(puerto2) {}

	~Accepter()
	{
		sock_prot1.cerrar();
		sock_prot2.cerrar();
	}

	void ejecutar()
	{
		correr = true;
		sock_prot1.escuchar(puerto1.c_str(), MAX_COLA);
		sock_prot2.escuchar(puerto2.c_str(), MAX_COLA);
		while (correr)
		{
			bool exito = aceptar_conexion();
			if (exito) cout << "Conexion exitosa desde un cliente" << endl;
			else cout << "Conexion fallida desde un cliente" << endl;
		}
	}
	void stop() {
		Lock lock_temp (mutex);
		correr = false;
		for (size_t i = 0; i < comunicadores.size(); ++i) {
			comunicadores[i]->stop();
		}
		sock_prot1.cerrar();
		sock_prot2.cerrar();
	}
	/**@brief acepta una conexion y crea threads de acuerdo al mensaje
	 * @details utiliza al socket para aceptar/rechazar conexiones entrantes
	 * y si es valido el usuario genera una instancia de la clase ServerComunicator
	 *  y lo ejecuta en otro thread*/
	bool aceptar_conexion()
	{
		cout << "Aguardando conexion." << endl;
		int fd_nuevo_1 = sock_prot1.aceptar();
		if (fd_nuevo_1 < 0) return false;
		cout << "Conexion recibida, esperando login." << endl;
		// Recibo mensaje
		SocketProt sock1(fd_nuevo_1);
		PacketID login = FAIL;
		sock1.recibir_flag(login);
		if (login != LOGIN) return false; // Veo que sea el paquete correcto
		string usuario;
		string contrasenia;
		sock1.recibir_msg_c_prefijo(usuario, BYTES_USER_PASS);
		cout << usuario.length() << usuario << endl;
		sock1.recibir_msg_c_prefijo(contrasenia, BYTES_USER_PASS);
		bool login_correcto = base_datos_usu.usuario_contrasenia_correcto
				(usuario.c_str(), contrasenia.c_str());
		if (!login_correcto)
		{
			sock1.enviar_flag(FAIL);
			sock1.cerrar();
		}
		else
		{
			// Si mensaje bueno creo el otro socket
			sock1.enviar_flag(OK);
			int fd_nuevo_2 = sock_prot2.aceptar();
			if(fd_nuevo_2 < 0)
			{
				sock1.cerrar();
				return false;
			}
			ServerCommunicator* comu = new ServerCommunicator(dir, fd_nuevo_1, fd_nuevo_2);
			comunicadores.push_back(comu);
			comu->start();
		}
		// Nota: El delete de sock1 no llama a close, asi que con pasar su file descriptor esta todo en orden
		return true;
	}
};

#endif /* SERVER_ACCEPTER_H_ */
