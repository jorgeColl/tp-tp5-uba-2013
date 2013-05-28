#include "common_base_de_datos.h"
#include <dirent.h>			// Explorar directorios
#include <sys/stat.h>		// Stat
#include <cstring> 			// memcpy
#include <sstream>

bool BaseDeDatos::abrir(const std::string &directorio)
{
	this->directorio = directorio;
	pathArchivo = directorio;
	pathArchivo.append(NOMBRE_ARCH_DEF);
	archivo.open(pathArchivo.c_str(),std::ios::in | std::ios::out | std::ios::binary);
	if (!archivo.is_open())
	{
		archivo.open(pathArchivo.c_str(),std::ios::out | std::ios::binary);
		archivo.close();
		archivo.open(pathArchivo.c_str(),std::ios::in | std::ios::out | std::ios::binary);
		return false;
	}
	cargarARam();
	return true;
}

vector<Modificacion> BaseDeDatos::comparar_indices(fstream &otro)
{
	return vector<Modificacion>();
}

//----- Modificacion de archivos en el directorio

bool BaseDeDatos::abrir_para_escribir(const string& nombre_archivo, fstream &ofstream)
{
	string dir(directorio);
	dir.append(nombre_archivo);
	ofstream.open(dir.c_str(), ios::out | ios::binary);
	return (ofstream.is_open());
}

bool BaseDeDatos::abrir_para_escribir_temporal(const string& nombre_archivo, fstream &ofstream)
{
	string nombre(nombre_archivo);
	nombre.append(EXT_TMP);
	return abrir_para_escribir(nombre, ofstream);
}

bool BaseDeDatos::renombrar(const string &viejo_nombre,const string &nuevo_nombre) {
	string path1(directorio);
	string path2(directorio);
	path1.append(viejo_nombre);
	path2.append(nuevo_nombre);
	// A criterio si conviene levantar una excepcion si rename != 0
	return rename(path1.c_str(), path2.c_str());
}

bool BaseDeDatos::renombrar_temporal(const string &nombre_archivo)
{
	string viejo_nombre(nombre_archivo);
	viejo_nombre.append(EXT_TMP);
	return renombrar(viejo_nombre, nombre_archivo);
}

bool BaseDeDatos::eliminar_archivo(const string &nombre_archivo)
{
	// A criterio si conviene levantar una excepcion si rename != 0
	return remove( nombre_archivo.c_str() );
}

bool BaseDeDatos::abrir_para_leer(const string &nombre_archivo, fstream &ifstream)
{
	string path(directorio);
	path.append(nombre_archivo);
	ifstream.open(path.c_str(), ios::in | ios::binary);
	return ifstream.is_open();
}

//----- Registracion en el indice de eventos

std::vector<Modificacion> BaseDeDatos::comprobar_cambios_locales()
{
	std::vector<Modificacion> modifs;

	DIR* dir = opendir(directorio.c_str());
	if (dir == NULL) return modifs;
	struct dirent* dirEnt = readdir(dir);
	while(dirEnt != NULL)
	{
		string path(directorio);
		path.append(dirEnt->d_name);
		struct stat buf;
		int val = stat(path.c_str(), &buf);
		if (val == -1 || !S_ISREG(buf.st_mode)) //Veo que efectivamente fuera un archivo
		{
			dirEnt = readdir(dir); //Error, seguimos
			continue;
		}
		//Todo: hacer algo con esto
		RegistroIndice reg(string(dirEnt->d_name), buf.st_mtim.tv_sec, buf.st_size, string());
		dirEnt = readdir(dir);
	}
	closedir(dir);
	//Nota: No liberar dirent
	return modifs;
}

vector<Modificacion> comparar_indices(fstream &otro)
{
	//TODO: Terminar
	return vector<Modificacion>();
}



//----- Registracion en el indice de eventos

bool BaseDeDatos::registrar_nuevo(const string &nombre_archivo)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::registrar_eliminado(const string &nombre_archivo)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::registrar_modificado(const string &nombre_archivo)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::registrar_renombrado(const string &nombre_viejo, const string &nombre_nuevo)
{
	//TODO: Terminar
	return true;
}

//----- Metodos privados

void BaseDeDatos::cargarARam()
{
	indice.cargar(archivo);
}

//----- Registro Indice

BaseDeDatos::RegistroIndice::RegistroIndice(const string &nombre, time_t modif, off_t tam, const string &hash)
	: nombre(nombre), modif(modif), tam(tam), hash(hash) {}

BaseDeDatos::RegistroIndice::RegistroIndice(const char *bytes, uint8_t tamNombre) : modif(), tam()
{
	nombre.append(bytes, tamNombre);
	memcpy((void*) &modif,(void*) (bytes+tamNombre), sizeof(time_t));
	memcpy((void*) &tam,(void*)(bytes+tamNombre+sizeof(time_t)), sizeof(off_t));
	hash.append(bytes+tamNombre+sizeof(time_t)+sizeof(off_t), BYTES_HASH);
}

string BaseDeDatos::RegistroIndice::serializar()
{
	stringstream result;
	uint8_t tamString = nombre.size();
	result.write((char*)&tamString, BYTES_PREF_NOMBRE);
	result.write(nombre.c_str(), nombre.size());
	result.write((char*)&modif, sizeof(time_t));
	result.write((char*)&tam, sizeof(off_t));
	result.write(hash.c_str(), BYTES_HASH);
	return result.str();
}

size_t BaseDeDatos::RegistroIndice::tamMax()
{
	return NAME_MAX+sizeof(time_t)+sizeof(off_t)+BYTES_HASH;
}

size_t BaseDeDatos::RegistroIndice::tamReg(size_t prefijo)
{
	return prefijo+sizeof(time_t)+sizeof(off_t)+BYTES_HASH;
}

//----- Indice en ram

void BaseDeDatos::IndiceRam::cargar(fstream &arch)
{
	arch.seekg(0);
	char* buffer = new char[RegistroIndice::tamMax()];
	while(arch.good())
	{
		uint8_t prefijo;
		arch.read((char*)&prefijo,1);
		size_t tamReg = RegistroIndice::tamReg(prefijo);
		RegistroIndice reg(buffer, tamReg);
		almacenamiento.push_back(reg);
	}
	delete[] buffer;
}

void BaseDeDatos::IndiceRam::agregar(RegistroIndice &reg)
{
	almacenamiento.push_back(reg);
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarNombre(string &nombre)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->nombre == nombre) return &(*it);
	}
	return NULL;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarFecha(time_t fecha)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->modif == fecha) return &(*it);
	}
	return NULL;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarTam(off_t tam)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->tam == tam) lista.push_back(&(*it));
	}
	return lista;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarHash(string &hash)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->hash == hash) return &(*it);
	}
	return NULL;
}
