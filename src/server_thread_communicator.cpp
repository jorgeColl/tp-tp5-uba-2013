#include "server_thread_communicator.h"

ServerCommunicator::ServerCommunicator(const char* dir, int fd1, int fd2)  {
	// QUERIA PONER ESTO EN EL CONSTRUCTOR PERO NO PUDE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// : sock1(fd1), sock2(fd2)
	this->dir = dir;
	finish = false;
	// ESTO VIOLA OBJETOS
	sock1.sockfd = fd1;
	sock2.sockfd = fd2;
}

void ServerCommunicator::stop() {
	Lock temp (mutex);
	finish = true;
}
void procesar_flag(PacketID& flag){
	switch(flag){
	case(LOGIN):
			// este no deberia llegar nunca
			break;
	case(FAIL):
			// nose
			break;
	case(OK):
			//nose
			break;
	case(LOGOUT):
			// cerrar coneccion y salir
			break;
	case(MODIFICACION):
			//recibe modificacion
			// ejecuta modificacion
					break;
	case(ARCHIVO_ENTERO):
			// recibe y guarda el archivo
			break;
	case(ARCHIVO_PARTES):
			// guarda parte nueva en archivo ??
			break;
	case(HASHES_BLOQUES):
			// hemm nose que hace-> help!!!
			break;
	case(PEDIDO_INDICE):
			// manda el indice
			break;
	}
}
void ServerCommunicator::ejecutar() {
	// se comunica con el cliente
	bool coneccion = true;
	while(coneccion && finish==false) {
		PacketID flag;
		coneccion = sock1.recibir_flag(flag);
		procesar_flag(flag);
	}
	Lock temp (mutex);
}
