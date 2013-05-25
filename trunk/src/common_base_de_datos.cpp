#include "common_base_de_datos.h"
#include <dirent.h>
#include <sys/stat.h>

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
		//Todo: hacer algo con esto
		//buf.st_size; //Tamaño en bytes
		//buf.st_mtim; //ultima modificacion
		dirEnt = readdir(dir);
	}
	closedir(dir);
	//No liberar dirent
	return modifs;
}

bool BaseDeDatos::actualizar_indice()
{
	//TODO: Hacer
	return true;
}

std::vector<Modificacion> BaseDeDatos::comparar_indices()
{
	//TODO: Terminar
	return vector<Modificacion>();
}

void BaseDeDatos::cargarARam()
{

}

bool BaseDeDatos::aplicar_cambios_locales(Modificacion& mod)
{
	//TODO: Terminar
	return true;
}

bool BaseDeDatos::agregar_archivo(std::string nombre_archivo, std::string datos)
{
	return true;
}
bool BaseDeDatos::eliminar_archivo(std::string nombre_archivo)
{
	return true;
}

bool BaseDeDatos::modificar_archvivo()
{
	return true;
}
