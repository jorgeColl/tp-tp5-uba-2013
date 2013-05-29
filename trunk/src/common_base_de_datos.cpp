#include "common_base_de_datos.h"
#include <dirent.h>			// Explorar directorios
#include <sys/stat.h>		// Stat
#include <cstring> 			// memcpy
#include <sstream>
#include <stdexcept>
#include "common_hashing.h"
#include <iostream>

#include <stdexcept> 	// Excepciones genericas
#include <cstring>		// Para manejar los buffers
#include <cerrno> 		// Errores de C


bool BaseDeDatos::abrir(const string directorio)
{
	this->directorio.append(directorio);
	pathArchivo.append(directorio);
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
	path1+="/";
	path2+="/";
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
	string tmp(directorio);
	tmp+=nombre_archivo;
	int exito=remove( tmp.c_str() );
	cout<<"remove nombre: "<<tmp<<endl;
	cout<<"remove: "<<exito<<endl;
	if(exito == -1){
		throw std::runtime_error(strerror(errno));
	}

	return exito;
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
	try
	{
		//Armo el registro con el archivo, y lo agrego al indice en ram y fisico
		RegistroIndice reg(nombre_archivo, directorio); //Puede fallar si el archivo no es bueno
		if (!reg.calcularHash()) return false;
		indice.agregar(reg);
		return registrar_nuevo_fis(reg);
	}
	catch (exception &e) { return false; }
}

bool BaseDeDatos::registrar_eliminado(const string &nombre_archivo)
{
	//Busco el registro y lo elimino de ram y del fisico
	RegistroIndice *reg = indice.buscarNombre(nombre_archivo);
	indice.eliminar(*reg);
	return registrar_eliminado_fis(*reg);
}

bool BaseDeDatos::registrar_modificado(const string &nombre_archivo)
{
	//Busco el registro y le recalculo el hash, y luego lo pongo en ram y el fisico
	RegistroIndice* reg = indice.buscarNombre(nombre_archivo);
	if (!reg->calcularHash()) return false;
	indice.modificar(*reg);
	return registrar_modificado_fis(*reg);
}

bool BaseDeDatos::registrar_renombrado(const string &nombre_viejo, const string &nombre_nuevo)
{
	//Busco el reg y lo edito en ram y luego en el fisico
	RegistroIndice* reg = indice.buscarNombre(nombre_viejo);
	indice.renombrar(*reg, nombre_nuevo);
	return registrar_renombrado_fis(*reg, nombre_nuevo);
}

//----- Metodos privados

void BaseDeDatos::cargarARam()
{
	indice.cargar(archivo);
}

bool BaseDeDatos::registrar_nuevo_fis(RegistroIndice &reg)
{
	archivo.seekp(0, ios::end);
	reg.archOffset = archivo.tellp();
	archivo << reg.serializar();
	return true;
}

bool BaseDeDatos::registrar_eliminado_fis(const RegistroIndice &reg)
{
	//TODO: Eliminado logico y etc
	return true;
}

bool BaseDeDatos::registrar_modificado_fis(const RegistroIndice &reg)
{
	archivo.seekp(reg.archOffset, ios::beg);
	archivo << reg.serializar();
	return true;
}

bool BaseDeDatos::registrar_renombrado_fis(const RegistroIndice &reg, const string &nombre_nuevo)
{
	//Ojala fuera tan sencillo, el nombre nuevo puede ser más grande,
	//mejor borrar y volver a agregar
	//archivo.seekp(reg.archOffset, ios::beg);
	//archivo << reg.serializar();
	return true;
}


//----- Registro Indice

BaseDeDatos::RegistroIndice::RegistroIndice(const string &nombre, time_t modif,
		off_t tam, const string &hash)	: nombre(nombre), modif(modif),
		tam(tam), hash(hash), archOffset(-1) {}

BaseDeDatos::RegistroIndice::RegistroIndice(const char *bytes, uint8_t tamNombre,
		uint32_t archOffset)	: modif(), tam(), archOffset(archOffset)
{
	nombre.append(bytes, tamNombre);
	memcpy((void*) &modif,(void*) (bytes+tamNombre), sizeof(time_t));
	memcpy((void*) &tam,(void*)(bytes+tamNombre+sizeof(time_t)), sizeof(off_t));
	hash.append(bytes+tamNombre+sizeof(time_t)+sizeof(off_t), BYTES_HASH);
}

BaseDeDatos::RegistroIndice::RegistroIndice(const string &nombre_archivo,
		const string &dir) : archOffset(-1)

{
	string path(dir);
	path.append(nombre_archivo);
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

bool BaseDeDatos::RegistroIndice::calcularHash()
{
	return MD5_arch(nombre, hash);
}

bool BaseDeDatos::RegistroIndice::operator==(const RegistroIndice &r2)
{
	return (nombre == r2.nombre && modif == r2.modif && tam == r2.tam
			&& hash == r2.hash && archOffset == r2.archOffset);
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
		RegistroIndice reg(buffer, tamReg, arch.tellg());
		almacenamiento.push_back(reg);
	}
	delete[] buffer;
}

void BaseDeDatos::IndiceRam::agregar(RegistroIndice &reg)
{
	almacenamiento.push_back(reg);
}

void BaseDeDatos::IndiceRam::eliminar(RegistroIndice &reg)
{
	almacenamiento.remove(reg);
}

void BaseDeDatos::IndiceRam::modificar(RegistroIndice &reg)
{
	reg.calcularHash();
}

void BaseDeDatos::IndiceRam::renombrar(RegistroIndice &reg, const string &nombre_nuevo)
{
	reg.nombre = nombre_nuevo;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarNombre(const string &nombre)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->nombre == nombre) return &(*it);
	}
	return NULL;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarFecha(const time_t fecha)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->modif == fecha) return &(*it);
	}
	return NULL;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarTam(const off_t tam)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->tam == tam) lista.push_back(&(*it));
	}
	return lista;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarHash(const string &hash)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->hash == hash) return &(*it);
	}
	return NULL;
}
