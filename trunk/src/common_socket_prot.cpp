#include <cstring>	// Memcpy
#include "common_socket_prot.h"
#include "common_base_de_datos.h" // BYTES_PREF_NOMBRE
#include "common_hashing.h"
#define TAM_BUFFER 4096

size_t SocketProt::cantidad_transmitida = 0;
Mutex SocketProt::mutex_cant_transmitida;

size_t SocketProt::cantidad_recibida = 0;
Mutex SocketProt::mutex_cant_recibida;

size_t SocketProt::get_and_reset_cantidad_recibida() {
	size_t aux;
	Lock lock_temp (SocketProt::mutex_cant_recibida);
	aux = cantidad_recibida;
	cantidad_recibida = 0;
	return aux;
}

SocketProt::SocketProt() : Socket(), contrasenia("default"){}

SocketProt::SocketProt (int socketfd) : Socket(socketfd), contrasenia("default") {}

void SocketProt::set_passw(string pass){
	contrasenia = pass;
}

void SocketProt::enviar_flag(const PacketID flag)
{
	string flag_a_enviar = (char*)&flag;
	this->firmar_mensaje(flag_a_enviar, contrasenia);
	enviarLen(flag_a_enviar.c_str(), flag_a_enviar.length());
	guardar_cant_transmitida(flag_a_enviar.length());
}

void SocketProt::recibir_flag(PacketID &flag)
{
	flag = ZERO;
	char flag_a_recibir [1 + BYTES_HASH];
	recibirLen(flag_a_recibir, 1 + BYTES_HASH);
	this->comprobar_firma(flag_a_recibir, contrasenia);

	flag = (PacketID) flag_a_recibir[0];
	guardar_cant_recibida(1 + BYTES_HASH);
}

void SocketProt::enviar_msg_c_prefijo(const string &msg, uint8_t bytes_para_prefijo)
{
	string men = msg;
	this->firmar_mensaje(men,contrasenia);

	size_t len = men.length();
	enviarLen((char*)&len, bytes_para_prefijo);
	enviarLen(men.c_str(), men.length());
	guardar_cant_transmitida(bytes_para_prefijo+men.size());
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

	this->comprobar_y_extraer_firma(msg,contrasenia);

	guardar_cant_recibida(msg.size());
}

void SocketProt::enviar_modif(const Modificacion &modif)
{
	size_t cant =0;
	string men = (char*)&modif.accion;
	this->firmar_mensaje(men,contrasenia);
	//enviar((void*)&(modif.accion),1);
	enviarLen(men.c_str(), men.length());

	enviar_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);

	enviar_msg_c_prefijo(modif.nombre_archivo_alt, BYTES_PREF_NOMBRE);


}

void SocketProt::recibir_modif(Modificacion &modif)
{
	modif.es_local = false;
	string men;
	char aux_men[1+BYTES_HASH];
	//recibir(&(modif.accion),1);
	recibirLen(aux_men,1+BYTES_HASH);

	recibir_msg_c_prefijo(modif.nombre_archivo, BYTES_PREF_NOMBRE);
	this->comprobar_y_extraer_firma(modif.nombre_archivo, contrasenia);
	recibir_msg_c_prefijo(modif.nombre_archivo_alt, BYTES_PREF_NOMBRE);
	this->comprobar_y_extraer_firma(modif.nombre_archivo, contrasenia);

	guardar_cant_recibida(1 + modif.nombre_archivo.size() + modif.nombre_archivo_alt.size() + 2*BYTES_PREF_NOMBRE);

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
}

void SocketProt::enviar_archivo(istream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ifstream
	arch.seekg(0,ios::end);
	streampos fin = arch.tellg();
	arch.seekg(0);
	enviarLen((const char*) &fin, sizeof(streampos)); //Envio el prefijo de longitud
	enviar_pedazo_archivo(arch, 0, fin);
}

void SocketProt::recibir_archivo(ostream &arch)
{
	//TODO: Optimizar viendo como funciona el buffer interno de ofstream
	streampos tam = 0;
	recibirLen((char*) &tam, sizeof(streampos));
	recibir_pedazo_archivo(arch, 0);
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
		cout << "Enviando bloque: " << *it << endl;
		enviar_pedazo_archivo(arch, *it*TAM_BLOQ, TAM_BLOQ);
	}
}

void SocketProt::recibir_edicion(istream &arch_orig, ostream &arch_temp)
{
	off_t bloques = 0;
	recibirLen((char*)&bloques, sizeof(off_t));
	cout << "Bloques:" << bloques << endl;
	list<off_t> bloqPedir;
	for (off_t i = 0; i < bloques; ++i) // Reviso todos los hashes que me llegan
	{
		cout << "Revisando hash:" << i << endl;
		char buffer[BYTES_HASH];
		recibirLen(buffer, BYTES_HASH);
		string hashRecibido(buffer, BYTES_HASH);
		string hashLocal;
		MD5_bloque(arch_orig, i*TAM_BLOQ, TAM_BLOQ, hashLocal);
		if (hashLocal != hashRecibido) bloqPedir.push_back(i);
	}
	//Envio la lista
	size_t tam = bloqPedir.size();
	cout << "Numero de bloques a pedir:" << tam << endl;
	enviarLen((char*)&tam, sizeof(size_t)); // Prefijo de longitud
	for(list<off_t>::iterator it = bloqPedir.begin(); it != bloqPedir.end(); ++it)
	{
		enviarLen((char*)&(*it), sizeof(off_t));
	}
	// Hacemos el "mechado" entre los dos archivos con lo que vamos recibiendo
	char buffer[TAM_BLOQ];
	for(off_t i = 0; i < bloques; ++i)
	{
		cout << "A escribir bloque: " << i << endl;
		off_t bloqNoLocal = -1;
		if (!bloqPedir.empty())	bloqNoLocal = bloqPedir.front();
		if (bloqNoLocal == i) // Si estamos en el bloque del archivo que me lleog por socket
		{
			// Copio lo que me llega por el socket
			cout << "Recibiendo bloque: " << i << endl;
			recibir_pedazo_archivo(arch_temp, i*TAM_BLOQ);
			bloqPedir.pop_front(); // Quito el bloque de la lista
		}
		else // Copio del original en el otro caso
		{
			arch_orig.clear();
			arch_orig.seekg(i*TAM_BLOQ, ios::beg);
			arch_orig.read(buffer, TAM_BLOQ);
			arch_temp.write(buffer, arch_orig.gcount());
		}
	}
}
void SocketProt::firmar_mensaje(std::string& mensaje, std::string contrasenia){
	mensaje += MD5_string(mensaje + contrasenia);
	//MD5_string(mensaje + contrasenia);
}

void SocketProt::comprobar_y_extraer_firma(std::string& mensaje,std::string contrasenia){
	string firma_recibida = mensaje.substr(mensaje.length()-BYTES_HASH);
	mensaje = mensaje.substr(0 , mensaje.length()-BYTES_HASH);
	if(firma_recibida != MD5_string(mensaje+contrasenia)){
		//throw std::runtime_error("firma no correcta de acuerdo al mensaje");
		cout<<"ERROR FIRMA NO BIEN ,mensaje entero"<<mensaje<< "hash esperado "<<mensaje.substr(mensaje.length()-BYTES_HASH)<<" recibido "<<MD5_string(mensaje+contrasenia)<<endl;
	}
}
void SocketProt::comprobar_firma(char* mensaje,string contrasenia){
	string men = mensaje;
	if( men.substr(men.length()-BYTES_HASH) != MD5_string(men.substr(0,men.length()-BYTES_HASH)+contrasenia) ) {
		//throw std::runtime_error("firma no correcta de acuerdo al mensaje");
		cout<<"ERROR FIRMA NO BIEN ,mensaje entero"<<mensaje<< "hash esperado "<<men.substr(men.length()-BYTES_HASH)<<" recibido "<<MD5_string(men+contrasenia)<<endl;
	}
}
void SocketProt::guardar_cant_transmitida(size_t cantidad){
	Lock aux (mutex_cant_transmitida);
	cantidad_transmitida += cantidad;
}

void SocketProt::guardar_cant_recibida(size_t cantidad) {
	Lock aux (mutex_cant_recibida);
	cantidad_recibida += cantidad;
}
