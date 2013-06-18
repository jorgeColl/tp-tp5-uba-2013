#include "common_util.h"
#include "defines.h"
#include <sstream>
#include <sys/stat.h>

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
