#include "common_hashing.h"
#include "hashlib/hl_md5wrapper.h"

bool MD5_arch(string &nombre_arch, string &hash)
{
	md5wrapper wrapper;
	try
	{
		hash = wrapper.getHashFromFile(nombre_arch);
	}
	catch(hlException &e)
	{
		return false;
	}
	return true;
}

bool MD5_bloques_arch(ifstream &arch, string &hash, size_t tamBloq)
{
	return false;
}


