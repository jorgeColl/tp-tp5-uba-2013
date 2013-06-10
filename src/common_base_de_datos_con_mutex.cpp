#include "common_base_de_datos_con_mutex.h"

BaseDeDatosConMutex::BaseDeDatosConMutex(const char* dir) :
		smpt(ArchMutexcer::generar_archmutexcer(dir)) {
	this->dir = dir;
}

void BaseDeDatosConMutex::abrir(const string &dir){
	Lock(this->mutx_indice);
	base_de_datos.abrir(dir);
}

bool BaseDeDatosConMutex::abrir_para_leer(const string &nombre_archivo, ifstream &ifstream) {
	Mutex* mu = smpt.data().get_mutex(nombre_archivo.c_str());
	mu->lock();
	pair <string,Mutex*> par( nombre_archivo, mu );
	mutexs_en_actividad.insert(par);
	return base_de_datos.abrir_para_leer(nombre_archivo, ifstream);
}
bool BaseDeDatosConMutex::cerrar_archivo(string& nombre_archivo, ofstream& fd) {
	fd.close();
	if(mutexs_en_actividad.count(nombre_archivo) == 0){
		throw ios_base::failure("error en BaseDeDatosConMutex::cerrar_archivo mutex no encontrado");
	}
	mutexs_en_actividad[nombre_archivo]->unlock();
	mutexs_en_actividad.erase(nombre_archivo);
	return true;
}
bool BaseDeDatosConMutex::cerrar_archivo(string& nombre_archivo, ifstream& fd) {
	fd.close();
	if(mutexs_en_actividad.count(nombre_archivo) == 0){
		throw ios_base::failure("error en BaseDeDatosConMutex::cerrar_archivo mutex no encontrado");
	}
	mutexs_en_actividad[nombre_archivo]->unlock();
	mutexs_en_actividad.erase(nombre_archivo);
	return true;
}
bool BaseDeDatosConMutex::abrir_para_escribir(const string& nombre_archivo, ofstream &ofstream){
	if (!smpt.data().is_mutex(nombre_archivo.c_str())){
		smpt.data().new_mutex(nombre_archivo.c_str());
	}
	Mutex* mu = smpt.data().get_mutex(nombre_archivo.c_str());
	mu->lock();
	pair <string,Mutex*> par( nombre_archivo, mu );
	mutexs_en_actividad.insert(par);
	return base_de_datos.abrir_para_escribir(nombre_archivo, ofstream);
}

bool BaseDeDatosConMutex::renombrar(const string &viejo_nombre, const string &nuevo_nombre) {
	smpt.data().new_mutex(nuevo_nombre.c_str());
	Lock (*smpt.data().get_mutex(nuevo_nombre.c_str()));
	Lock* lock = new Lock (*smpt.data().get_mutex(viejo_nombre.c_str()));
	bool exito = base_de_datos.renombrar(viejo_nombre,nuevo_nombre);
	if(exito){
		base_de_datos.registrar_renombrado(viejo_nombre, nuevo_nombre);
		delete(lock);
		smpt.data().delete_mutex(viejo_nombre.c_str());
	}else{
		delete(lock);
	}
	return exito;
}

bool BaseDeDatosConMutex::eliminar_archivo(const string &nombre_archivo){
	Lock* lock = new Lock(*smpt.data().get_mutex(nombre_archivo.c_str()));
	bool exito = base_de_datos.eliminar_archivo(nombre_archivo);

	if(exito){
		base_de_datos.registrar_eliminado(nombre_archivo);
		delete(lock);
		smpt.data().delete_mutex(nombre_archivo.c_str());
	}else{
		delete(lock);
	}
	return exito;
}

bool BaseDeDatosConMutex::copiar(const string &viejo_nombre, const string &nuevo_nombre){
	smpt.data().new_mutex(nuevo_nombre.c_str());
	Lock (*smpt.data().get_mutex(nuevo_nombre.c_str()));
	Lock* lock = new Lock (*smpt.data().get_mutex(viejo_nombre.c_str()));
	bool exito = base_de_datos.copiar(viejo_nombre, nuevo_nombre);

	if(exito){
		base_de_datos.registrar_copiado(viejo_nombre, nuevo_nombre);
		delete(lock);
		smpt.data().delete_mutex(viejo_nombre.c_str());
	}else{
		delete(lock);
	}
	return exito;
}

void BaseDeDatosConMutex::registrar_nuevo(const string &nombre_archivo){
	if (!smpt.data().is_mutex(nombre_archivo.c_str())){
		smpt.data().new_mutex(nombre_archivo.c_str());
	}
	Lock (*smpt.data().get_mutex(nombre_archivo.c_str()));
	Lock (this->mutx_indice);

	base_de_datos.registrar_nuevo(nombre_archivo);
}

//void BaseDeDatosConMutex::registrar_eliminado(const string &nombre_archivo);

//void BaseDeDatosConMutex::registrar_modificado(const string &nombre_archivo);



