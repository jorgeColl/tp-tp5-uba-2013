#include "common_hashing.h"
#include "hashlib/hl_md5wrapper.h"

bool MD5_arch(const string &path_arch, const string &password, string &hash)
{
	md5wrapper wrapper;
	try
	{
		hash = wrapper.getHashFromFile(path_arch);
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


