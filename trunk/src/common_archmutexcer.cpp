#include "common_arch_mutexcer.h"
#include "common_util.h"
#include <utility>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
// hago algo raro para que compile todo., es probable q en el .h solo halla declarado todo. los bichos pero nada mas
//std::vector<ArchMutexcer*> ArchMutexcer::a_eliminar;
std::map<std::string, ArchMutexcer*> ArchMutexcer::hijitos;
std::map<ArchMutexcer*, size_t> ArchMutexcer::cant_hijitos;
Mutex ArchMutexcer::mutex_clase;
using namespace std;

ArchMutexcer* ArchMutexcer::generar_archmutexcer(const char* directorio) {
	Lock temp (ArchMutexcer::mutex_clase);
	if (ArchMutexcer::hijitos.count(directorio) == 1) {
		cout<<"ArchMutexcer::generar_archmutexcer(): instancia ya estaba creada"<<endl;
		ArchMutexcer::cant_hijitos[hijitos[directorio]]++;
		return ArchMutexcer::hijitos[directorio];
	}
	cout<<"ArchMutexcer::generar_archmutexcer():  NUEVA instancia creada"<<endl;
	ArchMutexcer* nuevo = new ArchMutexcer(directorio);
	nuevo->construir_mutexs();

	ArchMutexcer::hijitos[directorio] = nuevo;
	ArchMutexcer::cant_hijitos[hijitos[directorio]] = 1;
	return nuevo;
}
ArchMutexcer::ArchMutexcer(const char* dir) {
	this->dir = new string;
	(*this->dir) = dir;
	this->mutex_archivos = new std::map <std::string,Mutex*>;
}
void ArchMutexcer::construir_mutexs() {

	// lista todos los archivos en el dir
	// le hace un mutex para cada uno
	DIR* direct = opendir(dir->c_str());
	if (direct == NULL) {
		cout << "ArchMutexcer:: no hay dir valido ????" << endl;
	}
	struct dirent* dirEnt = readdir(direct);
	while (dirEnt != NULL) {
		string path = unirPath(*dir,dirEnt->d_name);
		//mutex_archivos[dirEnt->d_name] = new Mutex;

		if (esIgnorable(dirEnt->d_name) || !esArchivo(path)) {
			dirEnt = readdir(direct); //Error, seguimos
			continue;
		}

		// agrego al map
		cout<<"ArchMutexcer::construir_mutexs(): encontre nuevo archivo "<<dirEnt->d_name<<endl;
		Mutex* mu = new Mutex();
		(*this->mutex_archivos)[dirEnt->d_name] = mu;

		dirEnt = readdir(direct);
	}
	closedir(direct);
}
Mutex* ArchMutexcer::get_mutex(const char* dir_archivo) {
	Lock(this->mutex_loc);
	if (!is_mutex(dir_archivo)){
		new_mutex(dir_archivo);
	}
	/*if((*mutex_archivos).count(dir_archivo) == 0) {
		cout<<"ArchMutexcer: dir no encontrado, no puedo devolver mutex";
		return NULL;
	}*/
	return (*mutex_archivos)[dir_archivo];
}
void ArchMutexcer::new_mutex(const char* dir_nuevo_archivo) {
	if(mutex_archivos->count(dir_nuevo_archivo) == 1){
		throw ios_base::failure("Se trata de crear un nuevo mutex para un archivo que ya tiene un mutex asociado");
	}
	(*mutex_archivos)[dir_nuevo_archivo] = new Mutex;
}
void ArchMutexcer::delete_mutex(const char* dir_archivo_a_eliminar){
	Lock(this->mutex_loc);
	if(mutex_archivos->count(dir_archivo_a_eliminar) == 1){
		throw ios_base::failure("Se trata de eliminar un mutex que no existe");
	}
	Mutex* mutx = (*mutex_archivos)[dir_archivo_a_eliminar];
	delete(mutx);
	mutex_archivos->erase(dir_archivo_a_eliminar);
}
bool ArchMutexcer::is_mutex(const char* dir_archivo){
	return (this->mutex_archivos->count(dir_archivo)==1);
}
void ArchMutexcer::borrar() {
	Lock* lock = new Lock(this->mutex_loc);
	Lock(this->mutex_clase);
	// se pone para eliminarse , si solo queda él se elimina de verdad, sino se resta 1
	cout << "ArchMutexcer::~ArchMutexcer(): eliminando instancia" << endl;
	cout << "ArchMutexcer::~ArchMutexcer():cant instancias: "
			<< ArchMutexcer::cant_instacias(this) << endl;
	if (1 == ArchMutexcer::cant_instacias(this)) {
		delete(lock);
		delete this;
	} else {
		cout << "ArchMutexcer::~ArchMutexcer(): solo se resta 1" << endl;
		ArchMutexcer::restar_instancia(this);
	}
	delete(lock);
}
ArchMutexcer::~ArchMutexcer() {
	cout << "ArchMutexcer::~ArchMutexcer(): se elimina permanentemente" << endl;
	std::map<std::string, Mutex*>::iterator it;
	for (it = mutex_archivos->begin(); it != mutex_archivos->end(); it++) {
		cout << "eliminando mutex de: " << it->first << endl;
		delete it->second;
	}
	cout << "CANT ELIMINADA: " << ArchMutexcer::cant_hijitos.erase(this)
			<< endl;
	cout << "CANT ELIMINADA: " << ArchMutexcer::hijitos.erase(*dir) << endl;
	delete this->mutex_archivos;
	delete dir;
}
size_t ArchMutexcer::cant_instacias(ArchMutexcer* mutexcer) {
	return cant_hijitos[mutexcer];
}
void ArchMutexcer::restar_instancia(ArchMutexcer* mutexcer) {
	cant_hijitos[mutexcer]--;
}

ostream& operator<<(std::ostream& os, ArchMutexcer& archm) {
	os<<"directorio: "<<*archm.dir<<endl;
	std::map<std::string,Mutex*>::iterator it;
	for(it=archm.mutex_archivos->begin(); it!=archm.mutex_archivos->end();it++){
		os<<it->first<<endl;
	}
	return os;
}
SmartP::SmartP(const std::string& dir) {
	mu = ArchMutexcer::generar_archmutexcer(dir.c_str());
}

SmartP::SmartP(ArchMutexcer* mutx){
	mu = mutx;
}
ArchMutexcer& SmartP::data() {
	return *mu;
}
SmartP::~SmartP(){
	mu->borrar();
}
