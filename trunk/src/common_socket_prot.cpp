#include "common_socket_prot.h"


bool SocketProt::enviar_flag(const Packet flag)
{
	return true;
}

bool SocketProt::recibir_flag(Packet &flag)
{
	return true;
}

bool SocketProt::enviar_modif(const Modificacion &modif)
{
	return true;
}

bool SocketProt::recibir_modif(Modificacion &modif)
{
	return true;
}

bool SocketProt::enviar_pedazo_archivo(ifstream &arch, size_t offset, size_t len)
{
	return true;
}

bool SocketProt::recibir_pedazo_archivo(ofstream &arch, size_t offset, size_t len)
{
	return true;
}
bool SocketProt::enviar_archivo(ifstream &arch){

	return true;
}
bool SocketProt::recibir_archivo(std::ofstream& fd){

	return true;
}
