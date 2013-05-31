#include "common_base_de_datos.h"
#include <dirent.h>			// Explorar directorios
#include <sys/stat.h>		// Stat
#include <cstring> 			// memcpy
#include <sstream>
#include <stdexcept>
#include <iostream>

#include <stdexcept> 	// Excepciones genericas
#include <cstring>		// Para manejar los buffers
#include <cerrno> 		// Errores de C

#include "common_hashing.h"
#include "common_util.h"

bool BaseDeDatos::abrir(const string directorio)
{
	this->directorio.append(directorio);
	pathArchivo.append(directorio);
	pathArchivo += "/";
	pathArchivo += NOMBRE_ARCH_DEF;
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
	// Me fijo si los archivos que tenia indexado siguen en la carpeta
	list<string> archIndexados = indice.devolverNombres();
	for (list<string>::iterator it = archIndexados.begin(); it != archIndexados.end(); ++it)
	{
		// Vemos si el archivo aun existe y sino lo marcamos como eliminado
		// Nota: Más abajo nos fijamos si fue renombrado en vez de borrado
		if (!esArchivo(directorio,*it)) // No existe
		{
			Modificacion modif(MANDAR_A_BORRAR_ARCHIVO, *it);
			modifs.push_back(modif);
		}
	}
	// Reviso que tal los archivos que ya existen
	DIR* dir = opendir(directorio.c_str());
	if (dir == NULL) return modifs;
	struct dirent* dirEnt = readdir(dir);
	while(dirEnt != NULL) // Mientras tenga archivos
	{
		string nombre(dirEnt->d_name);
		if (nombre == "" || nombre[0] == '.') //Archivos a ignorar
		{
			dirEnt = readdir(dir);
			continue;
		}
		string path(directorio);
		path += "/";
		path += nombre;
		struct stat buf;
		int val = stat(path.c_str(), &buf);
		if (val == -1 || !S_ISREG(buf.st_mode)) //Veo que efectivamente es un archivo
		{
			dirEnt = readdir(dir); //No era archivo o hubo un error, seguimos
			continue;
		}
		RegistroIndice* esta = indice.buscarNombre(nombre);
		if (esta) // Ya estaba indexado, vemos si fue modificado
		{
			// Se encontro archivo de mismo nombre y distinta fecha modif y distinto hash, entonces es una modif
			if (buf.st_mtim.tv_sec != esta->modif && esta->hash != MD5_arch(path,password))
			{
				MD5_arch(path, password); // Me fijo que el hash sea distinto
				Modificacion modif(SUBIR_MOD_ARCHIVO, esta->nombre);
				modifs.push_back(modif);
			}
		}
		if (!esta) // No estaba indexado
		{
			bool match = false;
			list<RegistroIndice*> matches = indice.buscarTam(buf.st_size); // Busco cambio de nombre por tam
			string hash = MD5_arch(path, password);
			for (list<RegistroIndice*>::iterator it = matches.begin(); it != matches.end(); ++it)
			{
				if ((*it)->hash == hash) //El hash es el mismo entonces el archivo es el mismo con otro nombre
				{
					// Primero me fijo si aun existe el archivo de viejo nombre, si es el caso es copia
					if (esArchivo(path))
					{
						Modificacion modif(MANDAR_COPIA_ARCHIVO, nombre, (*it)->nombre);
						modifs.push_back(modif);
					}
					else // Como no existe, en vez de copia es renombre
					{
						Modificacion modif(MANDAR_RENOMBRE_ARCHIVO, nombre, (*it)->nombre);
						modifs.push_back(modif);
					}
					match = true;
				}
			}
			if (!match) // No hubo matches, entonces el archivo es nuevo
			{
				Modificacion modif(SUBIR_NUEVO_ARCHIVO, nombre);
				modifs.push_back(modif);
			}
		}
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
		if (!reg.calcularHash(directorio,password,reg.hash)) return false;
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
	if (!reg->calcularHash(directorio,password,reg->hash)) return false;
	indice.modificar(*reg, password, directorio);
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
