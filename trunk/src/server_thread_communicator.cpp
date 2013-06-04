#include "server_thread_communicator.h"

ServerCommunicator::ServerCommunicator(const char* dir, int fd1, int fd2)
	: Controlador(dir, fd1, fd2), Thread()
{
	base_de_datos.abrir(dir);
}

void ServerCommunicator::actuar_segun_modif_recibida(Modificacion &mod)
{
	switch(mod.accion)
	{
		case NUEVO:
			{
			ofstream destino;
			base_de_datos.abrir_para_escribir(mod.nombre_archivo, destino);
			sock1.recibir_archivo(destino);
			destino.close();
			}
			break;
		case BORRADO:
			break;
		case EDITADO:
			break;
		case RENOMBRADO:
			break;
		case COPIADO:
			break;
	}
	sock1.enviar_flag(OK);
}

void ServerCommunicator::procesar_flag(PacketID flag)
{
	switch(flag)
	{
		case(ZERO):
		case(LOGIN):
		case(FAIL):
		case(YA_APLICADA):
		case(OK):
				// Estos no deberian llegar nunca "sueltos", se ignoran
				break;
		case(LOGOUT):
				// cerrar coneccion y salir
				break;
		case(MODIFICACION):
				{
				Modificacion mod;
				sock1.recibir_modif(mod); // Recibe mod
				cout << mod << endl;
				actuar_segun_modif_recibida(mod); // Ejecuta mod
				}
				break;
		case(PEDIDO_ARCHIVO_ENTERO):
				// Devuelve un archivo
				break;
		case(PEDIDO_ARCHIVO_PARTES):
				// Devuelve "partes" de un archivo
				break;
		case(PEDIDO_HASHES_BLOQUES):
				// Devuelve todos los hashes de cada bloque del archivo
				break;
		case(PEDIDO_INDICE):
				// Devuelve el indice
				break;
	}
}
void ServerCommunicator::ejecutar() {
	// Se comunica con el cliente
	bool coneccion = true;
	PacketID flag = ZERO;
	while(coneccion && correr)
	{
		coneccion = sock1.recibir_flag(flag);
		procesar_flag(flag);
	}
	Lock temp (mutex); // Que hace esto aca?
}
