#include <cstring>	// Memcpy
#include "common_socket_prot.h"
#include "common_base_de_datos.h" // BYTES_PREF_NOMBRE
#include <cerrno> 		// Errores de C
#include <sys/socket.h> // Sockets
#include <netdb.h>		// Inet
#include <stdexcept> 	// Excepciones genericas
#include <syslog.h>

#include "common_hashing.h"
#define TAM_BUFFER 4096

/*size_t SocketProt::cantidad_transmitida = 0;
Mutex SocketProt::mutex_cant_transmitida;

size_t SocketProt::cantidad_recibida = 0;
Mutex SocketProt::mutex_cant_recibida;

size_t SocketProt::get_and_reset_cantidad_recibida() {
	size_t aux;
	Lock lock_temp (SocketProt::mutex_cant_recibida);
	aux = cantidad_recibida;
	cantidad_recibida = 0;
	return aux;
}*/

SocketProt::SocketProt() : Socket() , contrasenia(CONTR_DEFAULT) {
	md5.MD5Init(&ctx);
}

SocketProt::SocketProt (int socketfd) : Socket(socketfd) , contrasenia(CONTR_DEFAULT) {
	md5.MD5Init(&ctx);
}
void SocketProt::set_password(string pass){
	contrasenia = pass;
}
int SocketProt::enviar(void *msg, size_t len) {
	int aux = Socket::enviar(msg,len);
	md5.MD5Update(&ctx, (unsigned char*)msg, aux);
	return aux;
}
int SocketProt::recibir(void *msg, size_t len) {
	int aux = Socket::recibir(msg,len);
	md5.MD5Update(&ctx, (unsigned char*)msg, aux);
	return aux;
}
void SocketProt::enviar_firma() {
	syslog(LOG_DEBUG, "Enviando firma");
	unsigned char digest[BYTES_HASH];

	//md5.MD5Init(&ctx);
	md5.MD5Update(&ctx, (unsigned char*)contrasenia.c_str(), contrasenia.length());
	md5.MD5Final(digest,&ctx);
	enviarLen((const char*)digest,BYTES_HASH);
	md5.MD5Init(&ctx);

	// aca podria recibir un OK o FAIL
}
void SocketProt::comprobar_firma(){
	syslog(LOG_DEBUG, "Recibiendo firma y comprobando contra la local");
	unsigned char digest_recibido[BYTES_HASH];
	unsigned char digest_esperado[BYTES_HASH];

	//md5.MD5Init(&ctx);
	md5.MD5Update(&ctx, (unsigned char*)contrasenia.c_str(), contrasenia.length());
	md5.MD5Final(digest_esperado,&ctx);

	recibirLen((char*)digest_recibido,BYTES_HASH);
	md5.MD5Init(&ctx);
	for(size_t i=0; i<BYTES_HASH;++i){
		if(digest_esperado[i]!=digest_recibido[i]){
			throw std::runtime_error("Error de seguridad, la firma recibida no coincide con la esperada.");
			break;
		}
	}
	syslog(LOG_DEBUG, "Firma OK");
	// aca podria enviar un OK o FAIL para la firma
}
void SocketProt::enviar_flag(const PacketID flag)
{
	enviarLen((char*) &flag, 1);
	enviar_firma();
	//guardar_cant_transmitida(1);
}

void SocketProt::recibir_flag(PacketID &flag)
{
	flag = ZERO;
	recibirLen((char*)&flag, 1);
	comprobar_firma();
	//guardar_cant_recibida(1);
}

void SocketProt::enviar_msg_c_prefijo(const string &msg, uint8_t bytes_para_prefijo)
{
	size_t len = msg.length();
	enviarLen((char*)&len, bytes_para_prefijo);
	enviarLen(msg.c_str(), msg.length());

	//guardar_cant_transmitida(bytes_para_prefijo+msg.size());
}

void SocketProt::recibir_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo)
{
	char* buffer1 = new char[bytes_para_prefijo]; // Pido el prefijo como char[]
	recibirLen(buffer1, bytes_para_prefijo);
	size_t tam = 0;
	memcpy(&tam, buffer1, bytes_para_prefijo); // Copio bytes del char[] al size_t
	delete []buffer1;
	char* buffer2 = new char[tam];
	recibirLen(buffer2, tam); // Recibo los bytes
	msg.append(buffer2, tam);  // Append de los bytes
	delete []buffer2;


	//guardar_cant_recibida(msg.size());
}

void SocketProt::enviar_modif(const Modificacion &modif)
{
	enviar((void*)&(modif.accion),1);
	enviar_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	enviar_msg_c_prefijo(modif.nombre_alt_o_hash, BYTES_PREF_NOMBRE);
	enviar_firma();
	//guardar_cant_transmitida(1 + modif.nombre_archivo.size() + modif.nombre_archivo_alt.size() + 2*BYTES_PREF_NOMBRE);
}

void SocketProt::recibir_modif(Modificacion &modif)
{
	modif.es_local = false;
	recibir(&(modif.accion),1);
	recibir_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	recibir_msg_c_prefijo(modif.nombre_alt_o_hash, BYTES_PREF_NOMBRE);
	comprobar_firma();
	//guardar_cant_recibida(1 + modif.nombre_archivo.size() + modif.nombre_archivo_alt.size() + 2*BYTES_PREF_NOMBRE);
}

void SocketProt::enviar_pedazo_archivo(istream &arch, off_t offset, off_t len)
{
	arch.clear();
	arch.seekg(0, ios::end);
	off_t finArch = arch.tellg();
	off_t fin = len;
	if (offset+len > finArch) fin = finArch-offset;
	enviarLen((char*)&fin, sizeof(off_t));
	arch.clear();
	arch.seekg(offset, ios::beg);
	char buffer[TAM_BUFFER];
	while (fin > 0 && arch.good())
	{
		streamsize aEnviar = TAM_BUFFER;
		if (fin < TAM_BUFFER) aEnviar = fin;
		arch.read(buffer, aEnviar);
		streamsize leidos =  arch.gcount();
		enviarLen(buffer, leidos); //Envio el archivo
		fin -= leidos;
	}
	arch.clear();
	//enviar_firma();
}

void SocketProt::recibir_pedazo_archivo(ostream &arch, off_t offset)
{
	off_t tam = 0;
	recibirLen((char*)&tam, sizeof(off_t));
	arch.clear();
	arch.seekp(offset, ios::beg);
	char buffer[TAM_BUFFER];
	while (tam > 0)
	{
		streamsize aRecibir = TAM_BUFFER;
		if (tam < TAM_BUFFER) aRecibir = tam;
		recibirLen(buffer, aRecibir);  //Recibo el archivo
		arch.write(buffer, aRecibir);
		tam -= aRecibir;
	}
	//comprobar_firma();
}

void SocketProt::enviar_archivo(istream &arch)
{
	arch.seekg(0,ios::end);
	streampos fin = arch.tellg();
	arch.seekg(0);
	enviarLen((const char*) &fin, sizeof(streampos)); //Envio el prefijo de longitud
	enviar_pedazo_archivo(arch, 0, fin);
	enviar_firma();
}

void SocketProt::recibir_archivo(ostream &arch)
{
	streampos tam = 0;
	recibirLen((char*) &tam, sizeof(streampos));
	recibir_pedazo_archivo(arch, 0);
	comprobar_firma();
}

void SocketProt::enviar_edicion(istream &arch)
{
	arch.clear();
	arch.seekg(0, ios::end);
	off_t tam = arch.tellg();
	arch.seekg(0);
	arch.clear();
	off_t bloques = tam / TAM_BLOQ;
	if (tam % TAM_BLOQ != 0) ++bloques;
	enviarLen((char*)&bloques, sizeof(off_t)); // Mando la cantidad de bloques del archivo
	for (off_t i = 0; i < bloques; ++i) // Mando todos los hashes por bloques del archivo
	{
		string hash;
		MD5_bloque(arch, i*TAM_BLOQ, TAM_BLOQ, hash);
		enviarLen(hash.c_str(), BYTES_HASH);
	}
	// Espero que el servidor me mande la lista de bloques que necesita
	list<off_t> bloqMandar;
	size_t numBloqMandar = 0;
	recibirLen((char*)&numBloqMandar, sizeof(size_t));
	for (size_t i = 0; i < numBloqMandar; ++i)
	{
		off_t numBloq = 0;
		recibirLen((char*)&numBloq, sizeof(off_t));
		bloqMandar.push_back(numBloq);
	}
	// Envio esos bloques, en orden
	for(list<off_t>::iterator it = bloqMandar.begin(); it != bloqMandar.end(); ++it)
	{
		syslog(LOG_DEBUG, "Enviando bloque: %li", *it);
		enviar_pedazo_archivo(arch, *it*TAM_BLOQ, TAM_BLOQ);
	}
	enviar_firma();
}

void SocketProt::recibir_edicion(istream &arch_orig, ostream &arch_temp)
{
	off_t bloques = 0;
	recibirLen((char*)&bloques, sizeof(off_t));
	syslog(LOG_DEBUG, "Bloques: %li", bloques);
	list<off_t> bloqPedir;
	for (off_t i = 0; i < bloques; ++i) // Reviso todos los hashes que me llegan
	{
		syslog(LOG_DEBUG, "Revisando hash del bloque: %li", i);
		char buffer[BYTES_HASH];
		recibirLen(buffer, BYTES_HASH);
		string hashRecibido(buffer, BYTES_HASH);
		string hashLocal;
		MD5_bloque(arch_orig, i*TAM_BLOQ, TAM_BLOQ, hashLocal);
		if (hashLocal != hashRecibido) bloqPedir.push_back(i);
	}
	//Envio la lista
	size_t tam = bloqPedir.size();
	syslog(LOG_DEBUG, "Numero de bloques a pedir: %u", tam);
	enviarLen((char*)&tam, sizeof(size_t)); // Prefijo de longitud
	for(list<off_t>::iterator it = bloqPedir.begin(); it != bloqPedir.end(); ++it)
	{
		enviarLen((char*)&(*it), sizeof(off_t));
	}
	// Hacemos el "mechado" entre los dos archivos con lo que vamos recibiendo
	char buffer[TAM_BLOQ];
	for(off_t i = 0; i < bloques; ++i)
	{
		syslog(LOG_DEBUG, "A escribir bloque: %li", i);
		off_t bloqNoLocal = -1;
		if (!bloqPedir.empty())	bloqNoLocal = bloqPedir.front();
		if (bloqNoLocal == i) // Si estamos en el bloque del archivo que me lleog por socket
		{
			// Copio lo que me llega por el socket
			syslog(LOG_DEBUG, "Recibiendo por socket bloque: %li", i );
			recibir_pedazo_archivo(arch_temp, i*TAM_BLOQ);
			bloqPedir.pop_front(); // Quito el bloque de la lista
		}
		else // Copio del original en el otro caso
		{
			syslog(LOG_DEBUG, "Copiando bloque %li de archivo local", i );
			arch_orig.clear();
			arch_orig.seekg(i*TAM_BLOQ, ios::beg);
			arch_orig.read(buffer, TAM_BLOQ);
			arch_temp.write(buffer, arch_orig.gcount());
		}
	}
	comprobar_firma();
}

/*void SocketProt::guardar_cant_transmitida(size_t cantidad){
	Lock aux (mutex_cant_transmitida);
	cantidad_transmitida += cantidad;
}

void SocketProt::guardar_cant_recibida(size_t cantidad) {
	Lock aux (mutex_cant_recibida);
	cantidad_recibida += cantidad;
}*/
