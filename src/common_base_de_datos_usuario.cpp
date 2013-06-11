#include "common_base_de_datos_usuario.h"
#include <iostream>
using namespace std;

BaseDeDatosUsuario::BaseDeDatosUsuario(const string &path) : path(path) {}

void BaseDeDatosUsuario::abrir() {
	ifstream arch1;
	arch1.open(path.c_str(), arch1.in |arch1.out);
	if (!arch1.is_open())
	{
		arch1.close();		//throw std::ios_base::failure("El archivo no existe");
		ofstream temp;
		temp.open(path.c_str());
		temp.close();
	}
	arch1.close();
	ifstream arch;
	arch.open(path.c_str(), arch.in |arch.out);
	if (!arch.is_open()) {
		throw std::ios_base::failure("El archivo no existe");
	}
	string usu;
	string contr;
	char buffer[200];
	arch >> buffer;
	usu = buffer;
	arch >> buffer;
	contr = buffer;
	while (!arch.eof()) {
		usu_contr.insert(pair<string, string>(usu, contr));
		arch >> buffer;
		usu = buffer;
		arch >> buffer;
		contr = buffer;
	}
	arch.close();
	/*cout<<"estado map:"<<endl;
	std::map<string,string>::iterator it;
	for(it = usu_contr.begin();it!=usu_contr.end();it++){
		cout<<it->first<<" "<<it->second<<endl;
	}*/
}

void BaseDeDatosUsuario::setPath(const string &path)
{
	this->path = path;
}

/**@brief Verifica si el usuario y contraseña ingresados pertenecen a un usuario de la base de datos */
bool BaseDeDatosUsuario::usuario_contrasenia_correcto(const char* usu,
		const char* contrasenia) {
	const string usuario(usu);
	const string contr(contrasenia);
	if (usu_contr.count(usuario) == 0) {
		return false;
	}
	if (usu_contr[usuario] != contr) {
		return false;
	}
	return true;
}
/**@brief Agrega un usuario a la base de datos */
void BaseDeDatosUsuario::agregar_usuario(const char* usuario,
		const char* contrasenia) {
	string usu(usuario);
	string contr(contrasenia);
	// lo meto en ram
	usu_contr.insert(pair<string, string>(usu, contr));

}
/**@brief Elimina un usuari de la base de datos */
void BaseDeDatosUsuario::eliminar_usuario(const char* usuario) {
	string usu(usuario);
	if (usu_contr.count(usu) == 0) {
		throw std::ios_base::failure("El usuario que queres borrar no existe");
	}
	usu_contr.erase(usu);
	/*arch.open(dir, arch.in | arch.out);
	if (!arch.is_open()) {
		throw std::ios_base::failure("El archivo no existe");
	}*/
}
void BaseDeDatosUsuario::guardar_a_disco() {
	// lo meto en archivo
	string dir_aux(path.c_str());
	dir_aux+=".temp";
	ofstream archivo;
	archivo.open(dir_aux.c_str());
	if (!archivo.is_open()) {
		throw std::ios_base::failure("El archivo no existe");
	}
	std::map<string,string>::iterator it;
	string usuario;
	string contrasen;
	for(it = usu_contr.begin();it!=usu_contr.end();it++){
		usuario = it->first;
		contrasen = it->second;
		archivo.write(usuario.c_str(), usuario.length());
		archivo.put(' ');
		archivo.write(contrasen.c_str(), contrasen.length());
		archivo.put('\n');
	}

	archivo.close();
	int exito = rename(dir_aux.c_str(),path.c_str());
	if (exito == -1) {
		throw std::runtime_error(strerror(errno));
	}
}
