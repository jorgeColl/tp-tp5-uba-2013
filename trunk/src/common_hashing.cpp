#include "common_hashing.h"
#include "hashlib/hl_md5wrapper.h"

#define HASH_TAM 16

class md5wrapperRaw : public md5wrapper
{
	protected:
	virtual string hashIt()
	{
		// Crear el hash
		unsigned char buff[HASH_TAM] = "";
		md5->MD5Final((unsigned char*)buff,&ctx);
		// Devolver el string crudo, no en hex
		return string((char*)buff, HASH_TAM);
	}
};

bool MD5_arch(const string &path_arch, const string &password, string &hash)
{
	md5wrapperRaw wrapper;
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

string MD5_arch(const string &path_arch, const string &password)
{
	md5wrapperRaw wrapper;
	return wrapper.getHashFromFile(path_arch);
}

bool MD5_bloques_arch(ifstream &arch, string &hash, size_t tamBloq)
{
	return false;
}


