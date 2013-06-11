#include <cstring>	// Memcpy
#include "common_socket_prot.h"
#include "common_base_de_datos.h" // BYTES_PREF_NOMBRE

#define TAM_BUFFER 4096

SocketProt::SocketProt() : Socket() {}

SocketProt::SocketProt (int socketfd) : Socket(socketfd) {}

void SocketProt::enviar_flag(const PacketID flag)
{
	enviar((void*) &flag, 1);
}

void SocketProt::recibir_flag(PacketID &flag)
{
	flag = ZERO;
	recibir(&flag, 1);
}

void SocketProt::enviar_msg_c_prefijo(const string &msg, uint8_t bytes_para_prefijo)
{
	size_t len = msg.length();
	enviarLen((char*)&len, bytes_para_prefijo);
	enviarLen(msg.c_str(), msg.length());
}

void SocketProt::recibir_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo)
{
	char* buffer1 = new char[bytes_para_prefijo]; // Pido el prefijo como char[]
	recibirLen(buffer1, bytes_para_prefijo);
	size_t tam = 0;
	memcpy(&tam, buffer1, bytes_para_prefijo); // Copio bytes del char[] al size_t
	delete buffer1;
	char* buffer2 = new char[tam];
	recibirLen(buffer2, tam); // Recibo los bytes
	msg.append(buffer2, tam);  // Append de los bytes
	delete buffer2;
}

void SocketProt::enviar_modif(const Modificacion &modif)
{
	enviar((void*)&(modif.accion),1);
	enviar_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	enviar_msg_c_prefijo(modif.nombre_archivo_alt, BYTES_PREF_NOMBRE);
}

void SocketProt::recibir_modif(Modificacion &modif)
{
	modif.es_local = false;
	recibir(&(modif.accion),1);
	recibir_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	recibir_msg_c_prefijo(modif.nombre_archivo_alt, BYTES_PREF_NOMBRE);
}

void SocketProt::enviar_pedazo_archivo(istream &arch, size_t offset, size_t len)
{
	return;
}

void SocketProt::recibir_pedazo_archivo(ostream &arch, size_t offset, size_t len)
{
	return;
}

void SocketProt::enviar_archivo(istream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ifstream
	arch.seekg(0,ios::end);
	streampos fin = arch.tellg();
	arch.seekg(0);
	enviarLen((const char*) &fin, sizeof(streampos)); //Envio el prefijo de longitud
	char buffer[TAM_BUFFER];
	while (fin > 0)
	{
		streamsize aEnviar = TAM_BUFFER;
		if (fin < TAM_BUFFER) aEnviar = fin;
		arch.read(buffer, aEnviar);
		enviarLen(buffer, aEnviar); //Envio el archivo
		fin -= aEnviar;
	}
}

void SocketProt::recibir_archivo(ostream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ofstream
	streampos tam = 0;
	recibirLen((char*) &tam, sizeof(streampos));
	char buffer[TAM_BUFFER];
	while (tam > 0)
	{
		streamsize aRecibir = TAM_BUFFER;
		if (tam < TAM_BUFFER) aRecibir = tam;
		recibirLen(buffer, aRecibir);  //Recibo el archivo
		arch.write(buffer, aRecibir);
		tam -= aRecibir;
	}
}
