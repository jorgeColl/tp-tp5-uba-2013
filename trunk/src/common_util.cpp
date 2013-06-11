#include "common_util.h"
#include <sys/stat.h>

bool esArchivo(const string &path)
{
	struct stat buf;
	return (stat(path.c_str(), &buf) >= 0 && S_ISREG(buf.st_mode));
}

bool esArchivo(const string &dir, const string &nombre)
{
	string path(dir);
	path += "/";
	path += nombre;
	struct stat buf;
	return (stat(path.c_str(), &buf) >= 0 && S_ISREG(buf.st_mode));
}

bool esIgnorable(const string &nombre)
{
	if (nombre.length() == 0) return true;
	if (nombre[0] == '.') return true;
	if (nombre[0] == '~') return true;
	if (nombre[nombre.length()-1] == '~') return true;
	return false;
}

off_t tamArchivo(const string &dir, const string &nombre)
{
	struct stat buf;
	stat(unirPath(dir, nombre).c_str(), &buf);
	return buf.st_size;
}

string unirPath(const string dir, const string nombre)
{
	string path(dir);
	path += "/";
	path += nombre;
	return path;
}
