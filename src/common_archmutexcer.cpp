#include "common_arch_mutexcer.h"
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
// hago algo raro para que compile todo., es probable q en el .h solo halla declarado todo. los bichos pero nada mas
std::vector<ArchMutexcer*> ArchMutexcer::a_eliminar;
std::map<const char*, ArchMutexcer*> ArchMutexcer::hijitos;
std::map<ArchMutexcer*, size_t> ArchMutexcer::cant_hijitos;

using namespace std;

ArchMutexcer* ArchMutexcer::generar_archmutexcer(const char* dir) {
	if (ArchMutexcer::hijitos.count(dir) == 1) {
		cant_hijitos[hijitos[dir]]++;
		return ArchMutexcer::hijitos[dir];
	}
	ArchMutexcer* nuevo = new ArchMutexcer(dir);
	nuevo->construir_mutexs();
	cant_hijitos[hijitos[dir]] = 1;
	return nuevo;
}
ArchMutexcer::ArchMutexcer(const char* dir) {
	this->dir = dir;
}
void ArchMutexcer::construir_mutexs() {
	// lista todos los archivos en el dir
	// le hace un mutex para cada uno
	DIR* direct = opendir(dir);
	if (direct == NULL) {
		cout << "ArchMutexcer:: no hay dir valido ????" << endl;
	}
	struct dirent* dirEnt = readdir(direct);
	while (dirEnt != NULL) {
		string path(dir);
		path.append(dirEnt->d_name);
		struct stat buf;
		int val = stat(path.c_str(), &buf);
		if (val == -1 || !S_ISREG(buf.st_mode)) //Veo que efectivamente fuera un archivo
		{
			dirEnt = readdir(direct); //Error, seguimos
			continue;
		}

		// agrego al map
		mutex_archivos[dirEnt->d_name] = new Mutex;

		dirEnt = readdir(direct);
	}
	closedir(direct);
}
Mutex* ArchMutexcer::get_mutex(const char* dir_archivo) {
	return mutex_archivos[dir_archivo];
}
void ArchMutexcer::new_mutex(const char* dir_nuevo_archivo) {
	mutex_archivos[dir_nuevo_archivo] = new Mutex;
}
ArchMutexcer::~ArchMutexcer() {
	std::map<const char*, Mutex*>::iterator it;
	for (it = mutex_archivos.begin(); it != mutex_archivos.end(); it++) {
		delete it->second;
	}
	// se pone para eliminarse , si solo queda él se elimina de verdad, sino se resta 1
	if (1 == ArchMutexcer::cant_instacias(this)) {
		ArchMutexcer::hijitos.erase(dir);
		ArchMutexcer::a_eliminar.push_back(this);
	} else {
		ArchMutexcer::restar_instancia(this);
	}
}
size_t ArchMutexcer::cant_instacias(ArchMutexcer* mutexcer) {
	return cant_hijitos[mutexcer];
}
void ArchMutexcer::restar_instancia(ArchMutexcer* mutexcer) {
	cant_hijitos[mutexcer]--;
}

