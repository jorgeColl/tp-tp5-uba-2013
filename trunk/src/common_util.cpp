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
