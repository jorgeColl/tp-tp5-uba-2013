#include "common_base_de_datos.h"
#include "common_util.h"

void BaseDeDatos::IndiceRam::cargar(istream &arch)
{
	arch.seekg(0);
	char* buffer = new char[RegistroIndice::tamMax()];
	while(arch.good())
	{
		uint8_t prefijo = 0;
		while (arch.good() && prefijo == 0) // Si hay 0s de prefijo sigo leyendo
			arch.read((char*)&prefijo,BYTES_PREF_NOMBRE);
		if (!arch.good()) break;
		uint32_t offset = arch.tellg();
		arch.read(buffer,RegistroIndice::tamReg(prefijo));
		RegistroIndice reg(buffer, prefijo, offset - BYTES_PREF_NOMBRE);
		almacenamiento.push_back(reg);
	}
	delete[] buffer;
	arch.clear(); // Ponemos el archivo en estado bueno
	arch.seekg(0);
}

void BaseDeDatos::IndiceRam::agregar(RegistroIndice &reg)
{
	almacenamiento.push_back(reg);
}

void BaseDeDatos::IndiceRam::eliminar(RegistroIndice &reg)
{
	reg.valido = false;
	reg.modif = time(NULL);
}

void BaseDeDatos::IndiceRam::modificar(RegistroIndice &reg, const string &dir)
{
	reg.calcularHash(dir, reg.hash);
	reg.modif = fechaModificado(dir, reg.nombre);
}

void BaseDeDatos::IndiceRam::renombrar(RegistroIndice &reg, const string &nombre_nuevo)
{
	reg.nombre = nombre_nuevo;
	reg.valido = true;
	reg.archOffset = OFFSET_INVALIDO;
}

BaseDeDatos::RegistroIndice* BaseDeDatos::IndiceRam::buscarNombre(const string &nombre, bool valido)
{
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->nombre == nombre && it->valido == valido) return &(*it);
	}
	return NULL;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarFecha(const time_t fecha, bool valido)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->modif == fecha && it->valido == valido) lista.push_back(&(*it));
	}
	return lista;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarTam(const off_t tam, bool valido)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->tam == tam && it->valido == valido) lista.push_back(&(*it));
	}
	return lista;
}

list<BaseDeDatos::RegistroIndice*> BaseDeDatos::IndiceRam::buscarHash(const string &hash, bool valido)
{
	list<BaseDeDatos::RegistroIndice*> lista;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->hash == hash && it->valido == valido) lista.push_back(&(*it));
	}
	return lista;
}

list<string> BaseDeDatos::IndiceRam::devolverNombres(bool valido)
{
	list<string> nombres;
	for (list<RegistroIndice>::iterator it = almacenamiento.begin(); it != almacenamiento.end(); ++it)
	{
		if (it->valido == valido) nombres.push_back(it->nombre);
	}
	return nombres;
}

time_t BaseDeDatos::IndiceRam::devolverFecha(const string& nombre, bool valido)
{
	RegistroIndice* reg = buscarNombre(nombre, valido);
	return reg->modif;
}
