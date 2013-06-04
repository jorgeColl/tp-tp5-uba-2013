#include "server_thread_communicator.h"

ServerCommunicator::ServerCommunicator(const char* dir, int fd1, int fd2)
	: Controlador(dir, fd1, fd2), Thread() {}

void procesar_flag(PacketID& flag){
	switch(flag){
	case(ZERO):
	case(LOGIN):
			// Estos no deberian llegar nunca, se ignoran
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
	// Se comunica con el cliente
	bool coneccion = true;
	while(coneccion && correr)
	{
		PacketID flag;
		coneccion = sock1.recibir_flag(flag);
		procesar_flag(flag);
	}
	Lock temp (mutex); // Que hace esto aca?
}
