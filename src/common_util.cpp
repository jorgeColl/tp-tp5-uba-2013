#include "common_util.h"
#include "defines.h"
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdexcept>
#include <iostream>
#include <syslog.h>
bool esArchivo(const string &path)
{
	struct stat buf;
	return (stat(path.c_str(), &buf) >= 0 && S_ISREG(buf.st_mode));
}

bool esArchivo(const string &dir, const string &nombre)
{
	string path = unirPath(dir,nombre);
	struct stat buf;
	return (stat(path.c_str(), &buf) >= 0 && S_ISREG(buf.st_mode));
}

off_t tamArchivo(const string &dir, const string &nombre)
{
	string path = unirPath(dir,nombre);
	struct stat buf;
	stat(path.c_str(), &buf);
	return buf.st_size;
}
bool esIgnorable2(const string &nombre)
{
	if (nombre.length() == 0) return true;
	if (nombre[0] == '.' && nombre[1]=='.') return true;
	if (nombre[0] == '~') return true;
	if (nombre[nombre.length()-1] == '~') return true;
	if (nombre[nombre.length()-1] == '.') return true;
	return false;
}

off_t __tamCarpeta(const char* direct) {
	//struct stat buf;
	//stat(dir, &buf);
	//return buf.st_size;
	off_t tam =0;
	DIR* dir = opendir(direct);
	if (dir == NULL) {
		syslog(LOG_ERR, "Directorio invalido: %s", direct);
		sleep(3);
		return 0;
	}

	struct dirent* dirEnt = readdir(dir);

	while (dirEnt != NULL) // Mientras tenga archivos
	{
		string nombre(dirEnt->d_name);
		string path = unirPath(direct, nombre);
		struct stat buf;
		int val = stat(path.c_str(), &buf);
		if(dirEnt->d_type==DT_DIR){
			if (val != -1 && !esIgnorable2(path)) //Veo que efectivamente es un archivo
			{
				syslog(LOG_DEBUG,"Tam de carpeta: Se entra al directorio: %s", path.c_str());
				tam+=buf.st_size;
				tam+=tamCarpeta(path.c_str());
			}
		}else{
			if (val != -1 && S_ISREG(buf.st_mode)){ //Veo que efectivamente es un archivo
				tam+=buf.st_size;
			}
		}
		dirEnt = readdir(dir);
	}
	return tam;
}
double tamCarpeta(const char* direct){
	return (__tamCarpeta(direct)/1024.00)/1024.0;
}
time_t fechaModificado(const string &dir, const string &nombre)
{
	string path = unirPath(dir,nombre);
	struct stat buf;
	stat(path.c_str(), &buf);
	return buf.st_mtim.tv_sec;
}

bool esIgnorable(const string &nombre)
{
	if (nombre.length() == 0) return true;
	if (nombre[0] == '.') return true;
	if (nombre[0] == '~') return true;
	if (nombre[nombre.length()-1] == '~') return true;
	return false;
}

string unirPath(const string dir, const string nombre)
{
	string path(dir);
	path += "/";
	path += nombre;
	return path;
}

string nombreConflictuado(const string &nombre)
{
	string conflic(NOMBRE_CONFLICTO);
	conflic += "_";
	stringstream fecha;
	fecha << time(NULL);
	conflic += fecha.str();
	conflic += "_";
	conflic += nombre;
	return conflic;
}
