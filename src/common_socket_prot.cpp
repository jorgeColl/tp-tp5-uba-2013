#include "common_socket_prot.h"

#define TAM_BUFFER 4096

bool SocketProt::enviar_flag(const PacketID flag)
{
	if (enviar((void*) &flag, 1) <= 0) return false;
	return true;
}

bool SocketProt::recibir_flag(PacketID &flag)
{
	if (recibir((void*) &flag, 1) <= 0) return false;
	return true;
}

bool SocketProt::enviar_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo)
{
	if (enviarLen((char*)msg.length(), bytes_para_prefijo) == false) return false;
	return enviarLen(msg.c_str(), msg.length());
}

bool SocketProt::recibir_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo)
{
	bool exito;
	char* buffer1 = new char[bytes_para_prefijo];
	exito = recibirLen(buffer1, bytes_para_prefijo);
	if (exito) msg.append(buffer1, bytes_para_prefijo);
	delete buffer1;
	if (!exito) return false;
	char* buffer2 = new char[msg.length()];
	exito = recibirLen(buffer2, msg.length());
	if (exito) msg.append(buffer2, msg.length());
	delete buffer2;
	return exito;
}

bool SocketProt::enviar_modif(const Modificacion &modif)
{
	return true;
}

bool SocketProt::recibir_modif(Modificacion &modif)
{
	return true;
}

bool SocketProt::enviar_pedazo_archivo(istream &arch, size_t offset, size_t len)
{
	return true;
}

bool SocketProt::recibir_pedazo_archivo(ostream &arch, size_t offset, size_t len)
{
	return true;
}

bool SocketProt::enviar_archivo(istream &arch)
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

bool SocketProt::recibir_archivo(ostream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ofstream
	streampos tam;
	if (!recibirLen((char*) tam, sizeof(streampos))) return false;
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
