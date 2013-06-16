#include "common_hashing.h"
#include "hashlib/hl_md5wrapper.h"
#include "defines.h"

class md5wrapperRaw : public md5wrapper
{
	protected:
	virtual string hashIt()
	{
		// Crear el hash
		unsigned char buff[BYTES_HASH] = "";
		md5->MD5Final((unsigned char*)buff,&ctx);
		// Devolver el string crudo, no en hex
		return string((char*)buff, BYTES_HASH);
	}
};

bool MD5_arch(const string &path_arch, string &hash)
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

string MD5_arch(const string &path_arch)
{
	md5wrapperRaw wrapper;
	return wrapper.getHashFromFile(path_arch);
}

bool MD5_bloque(istream &arch, off_t offset, off_t tamBloq, string &hash)
{
	arch.clear();
	arch.seekg(offset, ios::beg);
	MD5 md5;
	HL_MD5_CTX ctx;
	md5.MD5Init(&ctx);
	unsigned char buffer[TAM_BLOQ];
	off_t leidos = 0;
	while (leidos < tamBloq && arch.good())
	{
		arch.read((char*)buffer, TAM_BLOQ);
		streamsize recienLeidos = arch.gcount();
		md5.MD5Update(&ctx, buffer, recienLeidos);
		leidos += recienLeidos;
	}
	unsigned char digest[BYTES_HASH];
	md5.MD5Final(digest,&ctx);
	hash = string((char*)digest, BYTES_HASH);
	return false;
}


