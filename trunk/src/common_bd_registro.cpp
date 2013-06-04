#include "common_base_de_datos.h"
#include <dirent.h>			// Explorar directorios
#include <sys/stat.h>		// Stat
#include <cstring> 			// memcpy
#include <stdexcept> 		// Excepciones genericas
#include <sstream>
#include "common_hashing.h"

BaseDeDatos::RegistroIndice::RegistroIndice(const string &nombre, time_t modif,
		off_t tam, const string &hash)	: nombre(nombre), modif(modif),
		tam(tam), hash(hash), archOffset(-1) {}

BaseDeDatos::RegistroIndice::RegistroIndice(const char *bytes, uint8_t tamNombre,
		uint32_t archOffset)	: modif(0), tam(0), archOffset(archOffset)
{
	nombre.append(bytes, tamNombre);
	memcpy(&modif,(bytes+tamNombre), sizeof(time_t));
	memcpy(&tam,(bytes+tamNombre+sizeof(time_t)), sizeof(off_t));
	hash.append(bytes+tamNombre+sizeof(time_t)+sizeof(off_t), BYTES_HASH);
}

BaseDeDatos::RegistroIndice::RegistroIndice(const string &nombre_archivo,
		const string &dir) : archOffset(-1)
{
	string path(dir);
	path += "/";
	path += nombre_archivo;
	struct stat buf;
	int val = stat(path.c_str(), &buf);
	if (val == -1 || !S_ISREG(buf.st_mode)) throw invalid_argument("Archivo malo");
	nombre = string(nombre_archivo);
	modif = buf.st_mtim.tv_sec;
	tam = buf.st_size;
}

string BaseDeDatos::RegistroIndice::serializar() const
{
	stringstream result;
	uint8_t tamString = nombre.size();
	result.write((char*)&tamString, BYTES_PREF_NOMBRE);
	result.write(nombre.c_str(), nombre.size());
	result.write((char*)&modif, sizeof(time_t));
	result.write((char*)&tam, sizeof(off_t));
	result.write(hash.c_str(), BYTES_HASH);
	string ret(result.str());
	return ret;
}

size_t BaseDeDatos::RegistroIndice::tamMax()
{
	return NAME_MAX+sizeof(time_t)+sizeof(off_t)+BYTES_HASH;
}

size_t BaseDeDatos::RegistroIndice::tamReg(size_t prefijo)
{
	return prefijo+sizeof(time_t)+sizeof(off_t)+BYTES_HASH;
}

bool BaseDeDatos::RegistroIndice::calcularHash(const string &dir, const string &password, string &hash)
{
	string path(dir);
	path += "/";
	path += nombre;
	return MD5_arch(path, password, hash);
}

bool BaseDeDatos::RegistroIndice::operator==(const RegistroIndice &r2)
{
	return (nombre == r2.nombre && modif == r2.modif && tam == r2.tam
			&& hash == r2.hash && archOffset == r2.archOffset);
}


