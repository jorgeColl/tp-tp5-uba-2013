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
#include "common_threadM.h"
#include "common_mutex.h"

#define MAX_COLA 1000
// usa puerto 1
class Accepter: public ThreadM {
	bool finish;
	BaseDeDatosUsuario base_datos_usu;
	const char* dir;
	vector<ServerCommunicator*> comunicadores;
	const char* puerto1;
	const char* puerto2;
	SocketProt sock_prot1;
	SocketProt sock_prot2;
	Mutex mutex;
public:
	Accepter(const char* dir, const char* puerto1, const char* puerto2) :
			finish(false), base_datos_usu(dir), dir(dir) {
		this->puerto1 = puerto1;
		this->puerto2 = puerto2;
	}
	void ejecutar() {
		sock_prot1.escuchar(puerto1, MAX_COLA);
		sock_prot2.escuchar(puerto2, MAX_COLA);
		bool exito = true;
		while (exito && finish) {
			exito = aceptar_conexion();
		}
	}
	void stop() {
		Lock lock_temp (mutex);
		finish = false;
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
	bool aceptar_conexion() {

		int fd_nuevo_1 = sock_prot1.aceptar();
		if (fd_nuevo_1 < 0) {
			return false;
		}
		// recibo mensaje
		SocketProt temp;
		//si mensaje malo -> rechazo y cierro conexion
		string mensaje;
		temp.recibir_msg_c_prefijo(mensaje, 2); //FALTA TERMINAR
		string usuario;
		string contrasenia;
		size_t lugar = mensaje.find(' ');
		usuario = mensaje.substr(0, lugar);
		contrasenia = mensaje.substr(lugar + 1);
		bool login_correcto = base_datos_usu.usuario_contrasenia_correcto(
				usuario.c_str(), contrasenia.c_str());
		if (!login_correcto) {
			temp.enviar_flag(FAIL);
			temp.cerrar();

		} else {
			//si mensaje bueno
			// creo el otro socket
			int fd_nuevo_2 = sock_prot2.aceptar();
			if(fd_nuevo_2<0){
				SocketProt temp2(fd_nuevo_2);
				temp2.cerrar();
				temp.cerrar();
				return false;
			}
			ServerCommunicator* comu = new ServerCommunicator(dir, fd_nuevo_1, fd_nuevo_2);
			comunicadores.push_back(comu);
			comu->start();
		}
		return true;
	}
};

#endif /* SERVER_ACCEPTER_H_ */
