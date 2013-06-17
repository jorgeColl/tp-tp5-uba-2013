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

void BaseDeDatos::cerrar()
{
	archivo.close();
}

list<Modificacion> BaseDeDatos::comparar_indices(istream &otro)
{
	return comprobar_cambios_externos(otro);
}

list<Modificacion> BaseDeDatos::comprobar_cambios_externos(istream &indiceFuente)
{
	// Todo: Cambiar los devolvernombres por devolverRegistros
	IndiceRam indiceServer;
	indiceFuente.seekg(0, ios::beg);
	indiceServer.cargar(indiceFuente);
	bool es_local = false;
	list<Modificacion> modifs;
	// Me fijo si los archivos que tenia indexado siguen en la carpeta
	list<string> archBorrados = indiceServer.devolverNombres(false);
	// Itero por los archivos que deberian estar borrados, si existe, veo si lo tengo que borrar
	for (list<string>::iterator it = archBorrados.begin(); it != archBorrados.end(); ++it)
	{
		if (esArchivo(directorio,*it) // Existe y tal vez no deberia, miro la fecha para determinar eso
			&& fechaModificado(directorio, *it) < indiceServer.devolverFecha(*it, false)) // Existe y es viejo, lo borro
		{
			Modificacion modif(BORRADO, es_local, *it);
			modifs.push_back(modif);
		}
	}
	list<string> archIndexados = indiceServer.devolverNombres();
	// Itero por los archivos que existen, si no existe, lo pido como nuevo si es apropiado
	for (list<string>::iterator it = archIndexados.begin(); it != archIndexados.end(); ++it)
	{
		if (!esArchivo(directorio,*it)) // No existe, tal vez tengo que pedirlo
		{
			RegistroIndice* estaba = indice.buscarNombre(*it);
			if (!estaba || // Si no estaba indexado valido hay que pedirlo
				estaba->modif < indiceServer.devolverFecha(*it)) // Si estaba y recien se borro
				// antes de prender el cliente, miro si la fecha es más reciente que la de borrado
			{
				Modificacion modif(NUEVO, es_local, *it);
				modifs.push_back(modif);
			}
		}
		else // Si existe, veo que tengo que cambiarle
		{
			RegistroIndice* reg = indice.buscarNombre(*it);
			if (!reg) // No existe en el indice local pero existe como archivo fisico
					  // Por lo tanto es un archivo que el usuario recien creo/copio/renombro
					  // Pero tambien existe en el indice del server, asi que otro usuario lo commiteo
					  // Resuelvo el conflicto cambiandole el nombre al archivo del usuario local
					  // Y pidiendo un archivo nuevo
			{
				string nombrenuevo(NOMBRE_CONFLICTO);
				time_t fecha = time(NULL);
				struct tm *newtime = localtime(&fecha);
				nombrenuevo += asctime(newtime);
				nombrenuevo += *it;
				rename(reg->nombre.c_str(), nombrenuevo.c_str());
				Modificacion modif(NUEVO, es_local, *it);
				modifs.push_back(modif);
			}
			else // Ya estaba indexado, comparo las fechas y si es menor pido una modificacion
			{
				RegistroIndice* regExt = indiceServer.buscarNombre(*it);
				if (regExt->modif > reg->modif)
				{
					// Primero me fijo si no tengo ningun archivo del mismo hash para hacer una copia y listo
					list<RegistroIndice*> matches = indice.buscarHash(regExt->hash);
					RegistroIndice *aCopiar = NULL;
					if (!matches.empty()) aCopiar = matches.front(); // Agarro el primero, total es lo mismo
					if (aCopiar)
					{
						registrar_eliminado(*it); // Elimino el que tengo
						registrar_copiado(*it, aCopiar->nombre); // Copio
					}
					else // No encontre tal cosa, pido la modificacion
					{
						Modificacion modif(EDITADO, es_local, *it);
						modifs.push_back(modif);
					}
				}
			}
		}
	}
	return modifs;
}

/* Deprecated
list<Modificacion> BaseDeDatos::merge_modifs(list<Modificacion> &lista_externa, list<Modificacion> &lista_local)
{
	list<Modificacion> result;
	lista_externa.sort();
	lista_local.sort();
	list<Modificacion>::iterator it_ext = lista_externa.begin();
	list<Modificacion>::iterator it_loc = lista_local.begin();
	vector<string> renombrados;
	vector<string> renombres;
	// TODO: Analisar si esto funciona apropiadamente
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
	// Salimos del while, copio los que aun falten
	while(it_ext != lista_externa.end())
	{
		result.push_back(*it_ext);
		++it_ext;
	}
	while(it_loc != lista_local.end())
	{
		result.push_back(*it_loc);
		++it_loc;
	}
	return result;
}*/

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
	if (!esArchivo(pathViejo)) return true; // No se podia renombrar porque no existe
	bool exito = rename(pathViejo.c_str(), pathNuevo.c_str());
	if(exito != 0)
	{
		cout << "Error al renombrar el archivo " << viejo_nombre << " a "
				<< nuevo_nombre << ".Error: " << (strerror(errno)) << endl;
		return false;
	}
	return true;
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

bool BaseDeDatos::eliminar_archivo_temporal(const string &nombre_archivo)
{
	string viejo_nombre(nombre_archivo);
	viejo_nombre.append(EXT_TMP);
	return eliminar_archivo(viejo_nombre);
}

bool BaseDeDatos::eliminar_archivo(const string &nombre_archivo)
{
	// A criterio si conviene levantar una excepcion si rename != 0
	string path = unirPath(directorio, nombre_archivo);
	int exito = remove( path.c_str() );
	if(exito != 0)
	{
		cout << "Error al eliminar el archivo " << nombre_archivo <<
				".Error: " << (strerror(errno)) << endl;
		return false;
	}
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
	bool es_local = true;
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
			if (buf.st_mtim.tv_sec != esta->modif && esta->hash != MD5_arch(path))
			{
				MD5_arch(path); // Me fijo que el hash sea distinto
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
				hash = MD5_arch(path); // Si hubo matches, calculo el hash
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
	RegistroIndice* yaEstaba = indice.buscarNombre(nombre_archivo);
	if (yaEstaba) return;
	//Armo el registro con el archivo, y lo agrego al indice en ram y fisico
	RegistroIndice reg(nombre_archivo, directorio); //Puede fallar si el archivo no es bueno
	reg.calcularHash(directorio,reg.hash);
	RegistroIndice* regBorrado = indice.buscarNombre(nombre_archivo, false);
	if (regBorrado) // Ya existia pero borrado, entonces usamos el mismo offset
		reg.archOffset = regBorrado->archOffset;
	registrar_nuevo_fis(reg); // Primero lo agrego para obtener el offset correcto
	if (regBorrado) // Lo borramos de ram si estaba
		indice.eliminar(*regBorrado);
	indice.agregar(reg);
}

void BaseDeDatos::registrar_eliminado(const string &nombre_archivo)
{
	RegistroIndice* yaEstabaBorrado = indice.buscarNombre(nombre_archivo, false);
	if (yaEstabaBorrado) return;
	//Busco el registro y lo elimino de ram y del fisico
	RegistroIndice *reg = 0;
	reg = indice.buscarNombre(nombre_archivo);
	if (!reg) return;
	indice.eliminar(*reg); // Primero tengo que cambiar los valores del registro
	registrar_eliminado_fis(*reg);
}

void BaseDeDatos::registrar_editado(const string &nombre_archivo)
{
	//Busco el registro y le recalculo el hash, y luego lo pongo en ram y el fisico
	RegistroIndice* reg = indice.buscarNombre(nombre_archivo);
	if (!reg) return; // Si el registro no estaba que hago aca?
	indice.modificar(*reg, directorio); // Primero modifico para luego guardar
	registrar_editado_fis(*reg);
}

void BaseDeDatos::registrar_renombrado(const string &nombre_nuevo, const string &nombre_viejo)
{
	// Busco el reg viejo y lo elimino
	registrar_eliminado(nombre_viejo);
	// Registro como nuevo el nombre nuevo
	registrar_nuevo(nombre_nuevo); //Lo agrego como archivo nuevo tras cambiarle el nombre
}

void BaseDeDatos::registrar_copiado(const string &nombre_nuevo, const string &nombre_viejo)
{
	// Es lo mismo que registrar uno nuevo
	registrar_nuevo(nombre_nuevo);
}

bool BaseDeDatos::estaIndexado(const string &nombre_archivo, bool valido){
	return indice.buscarNombre(nombre_archivo, valido) != NULL;
}

//----- Metodos privados

void BaseDeDatos::cargarARam()
{
	indice.cargar(archivo);
}

void BaseDeDatos::registrar_nuevo_fis(RegistroIndice &reg)
{
	 // Si ya tenia offset, ya lo habia encontrado
	if (reg.archOffset != OFFSET_INVALIDO) // Lo reemplazo en el archivo
	{
		archivo.seekp(reg.archOffset, ios::beg);
		archivo << reg.serializar();
	}
	else // Sino hago un append al archivo
	{
		archivo.seekp(0, ios::end);
		reg.archOffset = archivo.tellp();
		archivo << reg.serializar();
	}
	if (!archivo.good()) throw runtime_error("Fallo el registro de un nuevo en el indice fisico.");
	archivo.flush(); // Seguridad
}

void BaseDeDatos::registrar_eliminado_fis(const RegistroIndice &reg)
{
	// Eliminado logico. Sumo 1 debido a los prefijos.
	archivo.seekp(reg.archOffset, ios::beg);
	archivo << reg.serializar();
	if (!archivo.good()) throw runtime_error("Fallo el borrado en el indice fisico.");
	archivo.flush(); // Seguridad
}

void BaseDeDatos::registrar_editado_fis(const RegistroIndice &reg)
{
	archivo.seekp(reg.archOffset);
	archivo << reg.serializar();
	if (!archivo.good()) throw runtime_error("Fallo el modificado en el indice fisico.");
	archivo.flush(); // Seguridad
}

BaseDeDatos::~BaseDeDatos()
{
	archivo.close();
}
