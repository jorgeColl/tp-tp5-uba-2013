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

void BaseDeDatos::abrir(const string &dir)
{
	directorio += dir;
	pathArchivo = unirPath(dir, NOMBRE_ARCH_IND);
	archivo.open(pathArchivo.c_str(), ios::in | ios::out | ios::binary);
	if (!archivo.is_open())
	{
		archivo.open(pathArchivo.c_str(),ios::out);
		archivo.close();
		archivo.open(pathArchivo.c_str(), ios::in | ios::out | ios::binary);
	}
	if (!archivo.good()) throw runtime_error("No pudo abrirse el archivo de indice.");
	cargarARam();
}

list<Modificacion> BaseDeDatos::comparar_indices(iostream &otro)
{
	// Flashie cualquiera
	// TODO: Hacerlo bien
	/*IndiceRam indiceServer;
	indiceServer.cargar(otro);
	list<Modificacion> mod_serv = comprobar_cambios(indiceServer, false);
	list<Modificacion> mod_cli = comprobar_cambios_locales();*/

	return list<Modificacion>();
}

list<Modificacion> BaseDeDatos::merge_modifs(list<Modificacion> &lista_externa, list<Modificacion> &lista_local)
{
	list<Modificacion> result;
	lista_externa.sort();
	lista_local.sort();
	list<Modificacion>::iterator it_ext = lista_externa.begin();
	list<Modificacion>::iterator it_loc = lista_local.begin();
	vector<string> renombrados;
	vector<string> renombres;
	// TODO: Menejar el gran problema que presentan los renombres usando solo los hashes como dato?
	while(it_ext != lista_externa.end() && it_loc != lista_local.end())
	{
		if (*it_ext < *it_loc)
		{
			result.push_back(*it_ext);
			++it_ext;
		}
		else if (*it_loc < *it_ext)
		{
			result.push_back(*it_loc);
			++it_loc;
		}
		else // Conflicto
		{
			list<Modificacion> solConflic = resolver_conflicto(*it_ext, *it_loc);
			for (list<Modificacion>::iterator it = solConflic.begin(); it != solConflic.end(); ++it)
			{
				result.push_back(*it);
			}
			++it_ext;
			++it_loc;
		}
	}
	return list<Modificacion>();
}

list<Modificacion> BaseDeDatos::resolver_conflicto(const Modificacion &modif_externa, const Modificacion &modif_local)
{
	// TODO: Mejor resolucion de conflictos generando/renombrando archivos conflictuados
	list<Modificacion> resultado;
	resultado.push_back(modif_externa);
	return resultado;
}

//----- Modificacion de archivos en el directorio

bool BaseDeDatos::abrir_para_escribir(const string& nombre_archivo, ofstream &ofstream)
{
	string path = unirPath(directorio, nombre_archivo);
	ofstream.open(path.c_str(), ios::out | ios::binary);
	return (ofstream.is_open());
}

bool BaseDeDatos::abrir_para_escribir_temporal(const string& nombre_archivo, ofstream &ofstream)
{
	string nombre(nombre_archivo);
	nombre.append(EXT_TMP);
	return abrir_para_escribir(nombre, ofstream);
}

bool BaseDeDatos::renombrar(const string &viejo_nombre,const string &nuevo_nombre) {
	string pathViejo = unirPath(directorio, viejo_nombre);
	string pathNuevo = unirPath(directorio, nuevo_nombre);
	// A criterio si conviene levantar una excepcion si rename != 0
	return (rename(pathViejo.c_str(), pathNuevo.c_str()) == 0);
}

bool BaseDeDatos::copiar(const string &viejo_nombre,const string &nuevo_nombre) {
	string pathOrigen = unirPath(directorio, viejo_nombre);
	string pathDestino = unirPath(directorio, nuevo_nombre);
	ofstream dest(pathDestino.c_str());
	ifstream orig(pathOrigen.c_str());
	bool exito = true;
	if (!dest.is_open() || !orig.is_open()) exito = false;
	if (exito) dest << orig.rdbuf(); // Magia
	dest.close();
	orig.close();
	return exito;
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
	string path = unirPath(directorio, nombre_archivo);
	int exito = remove( path.c_str() );
	if(exito != 0) return false;
	return true;
}

bool BaseDeDatos::abrir_para_leer(const string &nombre_archivo, ifstream &ifstream)
{
	string path = unirPath(directorio, nombre_archivo);
	ifstream.open(path.c_str(), ios::in | ios::binary);
	return ifstream.is_open();
}

//----- Registracion en el indice de eventos

list<Modificacion> BaseDeDatos::comprobar_cambios_locales()
{
	return comprobar_cambios(indice, true);
}

list<Modificacion> BaseDeDatos::comprobar_cambios(IndiceRam &indice, bool es_local)
{
	list<Modificacion> modifs;
	// Me fijo si los archivos que tenia indexado siguen en la carpeta
	list<string> archIndexados = indice.devolverNombres();
	for (list<string>::iterator it = archIndexados.begin(); it != archIndexados.end(); ++it)
	{
		// Vemos si el archivo aun existe y sino lo marcamos como eliminado
		// Nota: Más abajo nos fijamos si fue renombrado en vez de borrado
		if (!esArchivo(directorio,*it)) // No existe
		{
			Modificacion modif(BORRADO, es_local, *it);
			modifs.push_back(modif);
		}
	}
	// Reviso que tal los archivos que ya existen
	DIR* dir = opendir(directorio.c_str());
	if (dir == NULL) return modifs;
	struct dirent* dirEnt = readdir(dir);
	vector<string> renombrados;
	vector<string> renombre;
	while(dirEnt != NULL) // Mientras tenga archivos
	{
		string nombre(dirEnt->d_name);
		if (esIgnorable(nombre)) //Archivos a ignorar
		{
			dirEnt = readdir(dir);
			continue;
		}
		string path = unirPath(directorio, nombre);
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
				Modificacion modif(EDITADO, es_local, esta->nombre);
				modifs.push_back(modif);
			}
		}
		if (!esta) // No estaba indexado
		{
			bool match = false;
			list<RegistroIndice*> matches = indice.buscarTam(buf.st_size); // Busco cambio de nombre/copia por tam
			string hash;
			if (!matches.empty()) // Si se encontro el tamanio miro por los hashes
			{
				hash = MD5_arch(path, password); // Si hubo matches, calculo el hash
				matches = indice.buscarHash(hash); // Cambio los matches por los matches de hash
			}
			// Una pasada para renombres antes de la de copias
			for (list<RegistroIndice*>::iterator it = matches.begin(); it != matches.end() && !match; ++it)
			{
				string nomb = (*it)->nombre;
				string pathViejo = unirPath(directorio, nomb);
				for (list<Modificacion>::iterator it = modifs.begin(); it != modifs.end(); ++it)
				{
					if (it->accion == BORRADO) // No existe el archivo, entonces es renombre
					{
						Modificacion modif(RENOMBRADO, es_local, nombre, it->nombre_archivo);
						modifs.push_back(modif);
						renombrados.push_back(it->nombre_archivo);
						renombre.push_back(nombre);
						match = true;
						modifs.remove(*it);
						break;
					}
				}
			}
			// Una pasada para copias
			for (list<RegistroIndice*>::iterator it = matches.begin(); it != matches.end() && !match; ++it)
			{
				// Primero me fijo si aun existe el archivo de viejo nombre, si es el caso es copia
				string nomb = (*it)->nombre;
				for (size_t i = 0; i < renombrados.size(); ++i)
				{
					if (renombrados[i] == (*it)->nombre) nomb = renombre[i];
				}
				string pathViejo = unirPath(directorio, nomb);
				if (esArchivo(pathViejo))
				{
					Modificacion modif(COPIADO, es_local, nombre, nomb);
					modifs.push_back(modif);
					match = true;
					modifs.remove(Modificacion(BORRADO, es_local, (*it)->nombre));
					break;
				}
			}
			if (!match) // No hubo matches, entonces el archivo es nuevo
			{
				Modificacion modif(NUEVO, es_local, nombre);
				modifs.push_back(modif);
			}
		}
		dirEnt = readdir(dir);
	}
	closedir(dir);
	//Nota: No liberar dirent
	return modifs;
}

//----- Registracion en el indice de eventos

void BaseDeDatos::registrar_nuevo(const string &nombre_archivo)
{
	//Armo el registro con el archivo, y lo agrego al indice en ram y fisico
	RegistroIndice reg(nombre_archivo, directorio); //Puede fallar si el archivo no es bueno
	reg.calcularHash(directorio,password,reg.hash);
	registrar_nuevo_fis(reg); // Primero lo agrego para obtener el offset correcto
	indice.agregar(reg);
}

void BaseDeDatos::registrar_eliminado(const string &nombre_archivo)
{
	//Busco el registro y lo elimino de ram y del fisico
	RegistroIndice *reg = indice.buscarNombre(nombre_archivo);
	registrar_eliminado_fis(*reg); // Primero borro del fisico porque sino hay problemas de mem
	indice.eliminar(*reg);
}

void BaseDeDatos::registrar_modificado(const string &nombre_archivo)
{
	//Busco el registro y le recalculo el hash, y luego lo pongo en ram y el fisico
	RegistroIndice* reg = indice.buscarNombre(nombre_archivo);
	reg->calcularHash(directorio,password,reg->hash);
	indice.modificar(*reg, password, directorio); // Primero modifico para luego guardar
	registrar_modificado_fis(*reg);
}

void BaseDeDatos::registrar_renombrado(const string &nombre_nuevo, const string &nombre_viejo)
{
	//Busco el reg y lo edito en ram y luego en el fisico
	RegistroIndice* reg = indice.buscarNombre(nombre_viejo);
	registrar_eliminado_fis(*reg); //Lo borro del archivo si existe
	indice.renombrar(*reg, nombre_nuevo); // Cambio el nombre en ram
	registrar_nuevo_fis(*reg); //Lo agrego como archivo nuevo tras cambiarle el nombre
}

void BaseDeDatos::registrar_copiado(const string &nombre_nuevo, const string &nombre_viejo)
{
	//Busco el reg, hago una copia, le cambio el nombre, lo agrego en ram y luego en el fisico
	RegistroIndice* reg = indice.buscarNombre(nombre_viejo);
	RegistroIndice copia(*reg);
	copia.nombre = nombre_nuevo;
	indice.agregar(copia);
	registrar_nuevo_fis(copia);
}

//----- Metodos privados

void BaseDeDatos::cargarARam()
{
	indice.cargar(archivo);
	if (!archivo.good()) throw runtime_error("Fallo la carga del archivo indice.");
}

void BaseDeDatos::registrar_nuevo_fis(RegistroIndice &reg)
{
	archivo.seekp(0, ios::end);
	reg.archOffset = archivo.tellp();
	archivo << reg.serializar();
	if (!archivo.good()) throw runtime_error("Fallo el registro de un nuevo en el indice fisico.");
	archivo.flush(); // Seguridad
}

void BaseDeDatos::registrar_eliminado_fis(const RegistroIndice &reg)
{
	// Eliminado logico. Sumo 1 debido a los prefijos.
	archivo.seekp(reg.archOffset);
	size_t tam = RegistroIndice::tamReg(reg.nombre.size()) + BYTES_PREF_NOMBRE;
	char *zeros = new char[tam]();
	archivo.write(zeros, tam);
	delete[] zeros;
	if (!archivo.good()) throw runtime_error("Fallo el borrado en el indice fisico.");
	archivo.flush(); // Seguridad
}

void BaseDeDatos::registrar_modificado_fis(const RegistroIndice &reg)
{
	archivo.seekp(reg.archOffset);
	archivo << reg.serializar();
	if (!archivo.good()) throw runtime_error("Fallo el modificado en el indice fisico.");
	archivo.flush(); // Seguridad
}

BaseDeDatos::~BaseDeDatos()
{
	// TODO: Ver si hace falta reestructurar el archivo
	archivo.close();
}
