#include "common_socket_prot.h"

#define TAM_BUFFER 4096

bool SocketProt::enviar_flag(const Packet flag)
{
	if (enviar((void*) &flag, 1) <= 0) return false;
	return true;
}

bool SocketProt::recibir_flag(Packet &flag)
{
	if (recibir((void*) &flag, 1) <= 0) return false;
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

bool SocketProt::enviar_archivo(ifstream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ifstream
	arch.seekg(0,ios::end);
	streampos fin = arch.tellg();
	arch.seekg(0);
	if (!enviarLen((const char*) &fin, sizeof(streampos))) return false; //Envio el prefijo de longitud
	char buffer[TAM_BUFFER];
	while (fin > 0)
	{
		streamsize aEnviar = TAM_BUFFER;
		if (fin < TAM_BUFFER) aEnviar = fin;
		arch.read(buffer, aEnviar);
		if (!enviarLen(buffer, aEnviar)) return false; //Envio el archivo
		fin -= aEnviar;
	}
	return true;
}

bool SocketProt::recibir_archivo(std::ofstream &arch, streamsize tam)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ofstream
	char buffer[TAM_BUFFER];
	while (tam > 0)
	{
		streamsize aRecibir = TAM_BUFFER;
		if (tam < TAM_BUFFER) aRecibir = tam;
		if (!recibirLen(buffer, aRecibir)) return false;  //Recibo el archivo
		arch.write(buffer, aRecibir);
		tam -= aRecibir;
	}
	return true;
}
