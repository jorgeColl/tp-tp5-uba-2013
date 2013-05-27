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
std::ofstream& BaseDeDatos::generar_archivo_temp(std::string& nombre_archivo){
	string dir;
	dir+= directorio;
	dir+= nombre_archivo;
	dir+= ".temp";
	ofstream* fd = new ofstream(dir.c_str());
	return *fd;
}
bool BaseDeDatos::cerrar_exitosamente_archivo(std::string nombre_archivo, std::ofstream& fd) {
	fd.close();
	delete &fd;
	string dir;
	dir+= directorio;
	dir+= nombre_archivo;
	// ACA LO DEJO A CRITERIO SI COMBIENE LEVANTAR UNA EXCEPCION SI EXITO != 0
	int exito = rename((dir+=".temp").c_str(),dir.c_str());

	//ACA SE ACTUALIZA EL INDICE CON EL NUEVO ARCHIVO
		//FALTA HACER []

	return true;
}
std::ifstream& BaseDeDatos::abrir_para_leer(std::string& nombre_archivo) {
	string dir;
	dir+=directorio;
	dir+=nombre_archivo;
	ifstream* fd = new ifstream(dir.c_str());
	return *fd;
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

std::vector<Modificacion> BaseDeDatos::comparar_indices()
{
	//TODO: Terminar
	return vector<Modificacion>();
}

void BaseDeDatos::cargarARam()
{

}

bool BaseDeDatos::eliminar_archivo(std::string nombre_archivo)
{
	// ACA DEJO A CRITERIO SI COMBIENE LEVANTAR EXCEPCION SI EXITO != 0
	int exito = remove( nombre_archivo.c_str() );
	if (exito != 0){
		return false;
	}
	//ACA ACTUALIZA EL INDICE para indicar que el archivo se borró
		// FALTA HACER []
	return true;
}

bool BaseDeDatos::modificar_archvivo()
{
	return true;
}
